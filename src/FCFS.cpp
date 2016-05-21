/***
* FCFS.cpp - implementation of FCFS Scheduler class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Jan, 2016
*
* Purpose:
*       This file contains the implementation of the FCFS Scheduler class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "FCFS.h"
#include "Event.h"
#include "GlobalsAndTypes.h"
#include "yLog.h"

#include <string>
#include <sstream>
#include <algorithm>

using namespace std;
	
//----------------------------------------------------------------------

// Object's name
string FCFS::name_s = "FCFS";

//----------------------------------------------------------------------

FCFS::FCFS() {
		// Anything to do?
} // FCFS::FCFS()

//----------------------------------------------------------------------

FCFS::~FCFS() {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Releasing the remaining job pointers.");
	while(!jobs_q.empty()) {
		delete jobs_q.front();
		jobs_q.pop();
	}
} // SRPT::~SRPT()

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string FCFS::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event
bool FCFS::arrival_handler(ArrivalEvent * e) {
	Job * newjob = new Job(e->job_m);
	jobs_q.push(newjob);

	if (jobs_q.size() == 1) {
		newjob->isBeingExecuted_m = true;
		return true;
	}
	else
		return false;
}

//----------------------------------------------------------------------

// Handles a departure event
Job * FCFS::departure_handler(DepartureEvent * e) {
	//yLog::logtime(DEBUGLOG, __FUNCTION__, "");
	// Check if queue is empty
	if (jobs_q.empty()) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
						 while queue is empty.", e->job_id);
		return NULL;
	}
	// Check if this is an illegal departure
	if ((jobs_q.front())->getID() != e->job_id) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
						 while top of queue is job %d.", e->job_id, (jobs_q.front())->getID());
		return NULL;
	} 
	else if (!approximatelyEqual(jobs_q.front()->remsize_m, 0)) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job remaining size %10f, for job %d.", 
			jobs_q.front()->remsize_m, jobs_q.front()->getID());
		//return NULL;
	}

	// At this point, it is certain that this is a valid actual departure
	// Take care of removal and all that needs to be updated and logged
	Job * completedJob = jobs_q.front();	// Keep the pointer to pass to the logger, the logger should later free the job
	jobs_q.pop();

	// Mark the next job's execution
	if (!jobs_q.empty())
		jobs_q.front()->isBeingExecuted_m = true;

	// nextValidDepartureID();		//? Shall we make sure we invalidate its id?
	completedJob->departure_m = e->time;
	completedJob->isBeingExecuted_m = false;
	completedJob->isFinished_m = true;
	return completedJob;
}

//----------------------------------------------------------------------

// Handles a speed-change event
bool FCFS::speedchange_handler(SpeedChangeEvent * e) {
	// FCFS doesn't care about the speed change.
	return false;
}

//----------------------------------------------------------------------

// Returns the job-ID of the current job under execution
unsigned long FCFS::getExJob(double time) {
	if (jobs_q.empty()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
		return 0;
	} 
	else {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Job %d is under execution at time %f.", jobs_q.front()->getID(), time);
		if (jobs_q.front()->isBeingExecuted_m == false)
			yLog::logtime(ERRORLOG, __FUNCTION__, "Job %d is not marked as being executed at time %f.", jobs_q.front()->getID(), time);

		return jobs_q.front()->getID();
	}
}

//----------------------------------------------------------------------

// Creates a new departure Event object and retunrs its pointer
Event * FCFS::nextDeparture(double speed, double time) {
	if (jobs_q.empty()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
		return NULL;
	}

	if (!approximatelyEqual(time, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time);

	if (approximatelyEqual(speed, 0)) {	
		if (!approximatelyEqual(jobs_q.front()->remsize_m, 0))
			yLog::logtime(ERRORLOG, __FUNCTION__, "Running at speed 0 while there is a job in the system.");
		else
			return NULL;
	}

	double deptime = time + jobs_q.front()->remsize_m / speed;
	unsigned validid = nextValidDepartureID();
	Event * depevent = new DepartureEvent(deptime, Event_Type::DEPARTURE_EXPECTED, jobs_q.front()->getID(), validid);
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

void FCFS::updatePeriod(double time1, double time2, double speed, DESLogger * logger, PowerFunction * P) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	if (jobs_q.empty()) 
		yLog::logtime(DEBUGLOG, __FUNCTION__, "No job to update in interval [%10f, %10f].", time1, time2);
	else {
		Job * updatee = jobs_q.front();
		double takeout = (time2 - time1) * speed;
		
		// Update the record for the updatee job
		if (approximatelyEqual(updatee->remsize_m - takeout, 0))
			updatee->remsize_m = 0;
		else if (definitelyGreaterThan(takeout, updatee->remsize_m)) {
			yLog::logtime(ERRORLOG, __FUNCTION__, 
				"Job %d with size %f and takeout %f finishes before the end of interval [%10f, %10f].", 
					updatee->getID(), updatee->remsize_m, takeout,time1, time2);
			updatee->remsize_m = 0;
		}
		else
			updatee->remsize_m -= takeout;

		updatee->energyConsumed_m += logger->powerConsumed(speed, time2 - time1, P);
		updatee->executiontime_m += time2 - time1;
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Updating remaining work of job %d in interval [%10f, %10f].", updatee->getID(), time1, time2);
		if (logger != NULL)
			logger->execution_handler(vector<unsigned long> (1, updatee->getID()), vector<double> (1, speed), time1, time2);
	}
	lastUpdate_m = time2;
}	

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event * FCFS::bonusevent_handler(SchedulerEvent *, double speed) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event * FCFS::nextScheduler(double speed, double time) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------
