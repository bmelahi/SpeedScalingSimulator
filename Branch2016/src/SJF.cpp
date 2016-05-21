/***
* SJF.cpp - implementation of SJF Scheduler class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Jan, 2016
*
* Purpose:
*       This file contains the implementation of the SJF Scheduler class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "SJF.h"
#include "Event.h"
#include "GlobalsAndTypes.h"
#include "yLog.h"

#include <string>
#include <sstream>
#include <algorithm>

using namespace std;
	
//----------------------------------------------------------------------

// Object's name
string SJF::name_s = "SJF";

//----------------------------------------------------------------------

SJF::SJF() {
	current_job_m = NULL;
		// Anything to do?
} // SJF::SJF()

//----------------------------------------------------------------------

SJF::~SJF() {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Releasing the remaining job pointers.");
	while(!jobs_q.empty()) {
		delete jobs_q.top();
		jobs_q.pop();
	}
} // SRPT::~SRPT()

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string SJF::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event
bool SJF::arrival_handler(ArrivalEvent * e) {
	Job *newjob = new Job(e->job_m);

	if (NULL == current_job_m) {
		newjob->isBeingExecuted_m = true;
		current_job_m = newjob;
		return true;
	}
	else {
		jobs_q.push(newjob);
		return false;
	}
}

//----------------------------------------------------------------------

// Handles a departure event
Job * SJF::departure_handler(DepartureEvent * e) {
	//yLog::logtime(DEBUGLOG, __FUNCTION__, "");
	// Check if this is an illegal departure
	if (NULL == current_job_m) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
  											  						 while no job is currently under execution.", e->job_id);
		return NULL;
	}
	if (current_job_m->getID() != e->job_id) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
											  						 while current job under execution is %d.", e->job_id, current_job_m->getID());
		return NULL;
	}
	else if (!approximatelyEqual(current_job_m->remsize_m, 0)) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job remaining size %10f, for job %d.",
			current_job_m->remsize_m, current_job_m->getID());
		//return NULL;
	}

	// At this point, it is certain that this is a valid actual departure
	// Take care of removal and all that needs to be updated and logged
	Job * completedJob = current_job_m;	// Keep the pointer to pass to the logger, the logger should later free the job

	// Mark the next job's execution
	if (!jobs_q.empty()) {
		current_job_m = jobs_q.top();
		current_job_m->isBeingExecuted_m = true;
		jobs_q.pop();
	}
	else
		current_job_m = NULL;

	// nextValidDepartureID();		//? Shall we make sure we invalidate its id?
	completedJob->departure_m = e->time;
	completedJob->isBeingExecuted_m = false;
	completedJob->isFinished_m = true;
	return completedJob;
}

//----------------------------------------------------------------------

// Handles a speed-change event
bool SJF::speedchange_handler(SpeedChangeEvent * e) {
	// SJF doesn't care about the speed change.
	return false;
}

//----------------------------------------------------------------------

// Returns the job-ID of the current job under execution
unsigned long SJF::getExJob(double time) {
	if (current_job_m == NULL) {
		if (jobs_q.empty()) 
			yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
		
		else 
			yLog::logtime(ERRORLOG, __FUNCTION__, "There is no job currently under execution but the queue is not empty!");
		return 0;
	}
	else {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Job %d is under execution at time %f.", current_job_m->getID(), time);
		return current_job_m->getID();
	}
}

//----------------------------------------------------------------------

// Creates a new departure Event object and retunrs its pointer
Event * SJF::nextDeparture(double speed, double time) {
	if (NULL == current_job_m) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
		if (!jobs_q.empty()) 
			yLog::logtime(ERRORLOG, __FUNCTION__, "There is no job currently under execution but the queue is not empty!");
		return NULL;
	}

	if (!approximatelyEqual(time, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time);

	if (approximatelyEqual(speed, 0)) {	
		if (!approximatelyEqual(current_job_m->remsize_m, 0))
			yLog::logtime(ERRORLOG, __FUNCTION__, "Running at speed 0 while there is a job in the system.");
		else
			return NULL;
	}

	double deptime = time + current_job_m->remsize_m / speed;
	unsigned validid = nextValidDepartureID();
	Event * depevent = new DepartureEvent(deptime, Event_Type::DEPARTURE_EXPECTED, current_job_m->getID(), validid);
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

void SJF::updatePeriod(double time1, double time2, double speed, DESLogger * logger, PowerFunction * P) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	if (NULL == current_job_m) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "No job to update in interval [%10f, %10f].", time1, time2);
		if (!jobs_q.empty())
			yLog::logtime(ERRORLOG, __FUNCTION__, "There is no job currently under execution but the queue is not empty!");
	}

	else {
		double takeout = (time2 - time1) * speed;
		
		// Update the record for the updatee job
		if (approximatelyEqual(current_job_m->remsize_m - takeout, 0))
			current_job_m->remsize_m = 0;
		else if (definitelyGreaterThan(takeout, current_job_m->remsize_m)) {
			yLog::logtime(ERRORLOG, __FUNCTION__, 
				"Job %d with size %f and takeout %f finishes before the end of interval [%10f, %10f].", 
					current_job_m->getID(), current_job_m->remsize_m, takeout,time1, time2);
			current_job_m->remsize_m = 0;
		}
		else
			current_job_m->remsize_m -= takeout;

		current_job_m->energyConsumed_m += logger->powerConsumed(speed, time2 - time1, P);
		current_job_m->executiontime_m += time2 - time1;
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Updating remaining work of job %d in interval [%10f, %10f].", current_job_m->getID(), time1, time2);
		if (logger != NULL)
			logger->execution_handler(vector<unsigned long> (1, current_job_m->getID()), vector<double> (1, speed), time1, time2);
	}
	lastUpdate_m = time2;
}	

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event * SJF::bonusevent_handler(SchedulerEvent *, double speed) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event * SJF::nextScheduler(double speed, double time) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------
