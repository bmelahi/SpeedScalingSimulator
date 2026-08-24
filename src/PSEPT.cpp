/***
* PSEPT.cpp - implementation of Preemptive - SEPT(PSEPT) Scheduler class
*
*		Written by Mithun P
*		Last Updated: July 2022
*
* Purpose:
*       This file contains the implementation of the  Preemptive - SEPT(PSEPT) Scheduler class.
*
***/
#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "PSEPT.h"
#include "Event.h"
#include "GlobalsAndTypes.h"
#include "yLog.h"

#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

//----------------------------------------------------------------------

// Object's name
string PSEPT::name_s = "PSEPT";

//----------------------------------------------------------------------

PSEPT::PSEPT() {
	//current_job_m = NULL;
	// Anything to do?
} // PSEPT::PSEPT()

//----------------------------------------------------------------------

PSEPT::~PSEPT() {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Releasing the remaining job pointers.");
	while (!jobs_q.empty()) {
		delete jobs_q.top();
		jobs_q.pop();
	}
} // SRPT::~SRPT()

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string PSEPT::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event 
// Todo : make this preemtive
bool PSEPT::arrival_handler(ArrivalEvent* e) {

	Job* top = NULL;
	if (!jobs_q.empty())
		top = jobs_q.top();

	Job* newjob = new Job(e->job_m);
	jobs_q.push(newjob);

	// If the new job is on top of PSEPT queue, the there has been a context switch
	if (jobs_q.top()->getID() == newjob->getID()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Context switch, job %d preemts job %d.",
			newjob->getID(), (top == NULL) ? 0 : (top->getID()));
		// Mark the current top job is being executed
		// Mark the previous top job is no longer being executed
		jobs_q.top()->isBeingExecuted_m = true;
		if (top != NULL)
			top->isBeingExecuted_m = false;
		return true;
	}
	else
		return false;
}

//----------------------------------------------------------------------

// Handles a departure event
Job* PSEPT::departure_handler(DepartureEvent* e) {
	//yLog::logtime(DEBUGLOG, __FUNCTION__, "");
	// Check if this is an illegal departure
	if (jobs_q.empty()) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
  											  						 while no job is currently under execution.", e->job_id);
		return NULL;
	}
	if (jobs_q.top()->getID() != e->job_id) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
											  						 while top of the queue is %d.", e->job_id, (jobs_q.top())->getID());
		return NULL;
	}
	else if (!approximatelyEqual(jobs_q.top()->remsize_m, 0)) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job remaining size %10f, for job %d.",
			jobs_q.top()->remsize_m, jobs_q.top()->getID());
		//return NULL;
	}

	// At this point, it is certain that this is a valid actual departure
	// Take care of removal and all that needs to be updated and logged
	Job* completedJob = jobs_q.top();	// Keep the pointer to pass to the logger, the logger should later free the job
	jobs_q.pop();

	// Mark the next job's execution
	if (!jobs_q.empty())
		jobs_q.top()->isBeingExecuted_m = true;

	// nextValidDepartureID();		//? Shall we make sure we invalidate its id?
	completedJob->departure_m = e->time;
	completedJob->isBeingExecuted_m = false;
	completedJob->isFinished_m = true;
	return completedJob;
}

//----------------------------------------------------------------------

// Handles a speed-change event
bool PSEPT::speedchange_handler(SpeedChangeEvent* e) {
	// PSEPT doesn't care about the speed change.
	return false;
}

//----------------------------------------------------------------------

// Returns the job-ID of the current job under execution
unsigned long PSEPT::getExJob(double time) {
	
	if (jobs_q.empty()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
		return 0;
	}
	else {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Job %d is under execution at time %f.", jobs_q.top()->getID(), time);
		if (jobs_q.top()->isBeingExecuted_m == false)
			yLog::logtime(ERRORLOG, __FUNCTION__, "Job %d is not marked as being executed at time %f.", jobs_q.top()->getID(), time);

		return jobs_q.top()->getID();
	}
}

//----------------------------------------------------------------------

// Creates a new departure Event object and retunrs its pointer
Event* PSEPT::nextDeparture(double speed, double time) {
	if (jobs_q.empty()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
		return NULL;
	}

	if (!approximatelyEqual(time, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time);

	if (approximatelyEqual(speed, 0)) {
		if (!approximatelyEqual(jobs_q.top()->remsize_m, 0))
			yLog::logtime(ERRORLOG, __FUNCTION__, "Running at speed 0 while there is a job in the system.");
		else
			return NULL;
	}

	double deptime = time + jobs_q.top()->remsize_m / speed;
	unsigned validid = nextValidDepartureID();
	Event* depevent = new DepartureEvent(deptime, Event_Type::DEPARTURE_EXPECTED, jobs_q.top()->getID(), validid);
	yLog::logtime(DEBUGLOG, __FUNCTION__, "New DepartureEvent created for job %d at time %f, valid id %d.",
		depevent->job_id, depevent->time, depevent->validId_m);
	return(depevent);
}

//----------------------------------------------------------------------

/* Applies the passage of time.
	- Parameter one (time1) should match the
	internal value of previous update time.
	- Parameter three (speed), gives the speed
	during interval [time1, time2]
	- DESLogger * is used for simulation logs
	- Must call DESLogger.execution_handler   */

void PSEPT::updatePeriod(double time1, double time2, double speed, DESLogger* logger, PowerFunction* P) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	if (jobs_q.empty()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "No job to update in interval [%10f, %10f].", time1, time2);
		if (!jobs_q.empty())
			yLog::logtime(ERRORLOG, __FUNCTION__, "There is no job currently under execution but the queue is not empty!");
	}

	else {
		Job* updatee = jobs_q.top();
		double takeout = (time2 - time1) * speed;

		// Update the record for the updatee job
		if (approximatelyEqual(updatee->remsize_m - takeout, 0))
			updatee->remsize_m = 0;
		else if (definitelyGreaterThan(takeout, updatee->remsize_m)) {
			yLog::logtime(ERRORLOG, __FUNCTION__,
				"Job %d with size %f and takeout %f finishes before the end of interval [%10f, %10f].",
				updatee->getID(), updatee->remsize_m, takeout, time1, time2);
			updatee->remsize_m = 0;
		}
		else {
			updatee->remsize_m -= takeout;
			//current_job_m->estimatedremsize_m -= takeout;
		}
		updatee->energyConsumed_m += logger->powerConsumed(speed, time2 - time1, P);
		updatee->executiontime_m += time2 - time1;
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Updating remaining work of job %d in interval [%10f, %10f].", updatee->getID(), time1, time2);
		if (logger != NULL)
			logger->execution_handler(vector<unsigned long>(1, updatee->getID()), vector<double>(1, speed), time1, time2);
	}
	lastUpdate_m = time2;
}

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event* PSEPT::bonusevent_handler(SchedulerEvent*, double speed) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event* PSEPT::nextScheduler(double speed, double time) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------
