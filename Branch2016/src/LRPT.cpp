/***
* LRPT.cpp - implementation of LRPT Scheduler class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: April, 2013
*
* Purpose:
*       This file contains the implementation of the LRPT Scheduler class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "LRPT.h"
#include "Event.h"
#include "GlobalsAndTypes.h"
#include "PowerFunction.h"
#include "yLog.h"

#include <string>
#include <sstream>
#include <algorithm>

using namespace std;
	
//----------------------------------------------------------------------

// Object's name
string LRPT::name_s = "LRPT";

//----------------------------------------------------------------------

LRPT::LRPT() {
		// Anything to do?
} 

//----------------------------------------------------------------------

LRPT::~LRPT() {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Releasing the remaining job pointers.");
	while(!jobs_q.empty()) {
		delete (*jobs_q.begin());
		jobs_q.erase(jobs_q.begin());
	}
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string LRPT::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event
bool LRPT::arrival_handler(ArrivalEvent * e) {
	Job * newjob = new Job (e->job_m);
	jobs_q.insert(newjob);
	if (approximatelyEqual((*jobs_q.begin())->remsize_m, newjob->getSize()))
		newjob->isBeingExecuted_m = true;
	else
		newjob->isBeingExecuted_m = false;
	return newjob->isBeingExecuted_m;
}

//----------------------------------------------------------------------

// Handles a departure event
Job * LRPT::departure_handler(DepartureEvent * e) {
	// Check if queue is empty
	if (jobs_q.empty()) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
						 while queue is empty.", e->job_id);
		return NULL;
	}
	// Check if this is an illegal departure
	if ((*jobs_q.begin())->getID() != e->job_id) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
						 while top of queue is job %d.", e->job_id, (*jobs_q.begin())->getID());
		return NULL;
	} 
	else if (!approximatelyEqual((*jobs_q.begin())->remsize_m, 0)) {
			yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job remaining size %10f, for job %d.", 
				(*jobs_q.begin())->remsize_m, (*jobs_q.begin())->getID());
			//return NULL;
	}

	// At this point, it is certain that we are at then end of a busy period.
	// Flush the queue. Take care of removal and all that needs to be updated and logged for each job
	// Add the next departure

	Job * completedJob = (*jobs_q.begin());	// Keep the pointer to pass to the logger, the logger should later free the job
	jobs_q.erase(jobs_q.begin());

	completedJob->departure_m = e->time;
	completedJob->isBeingExecuted_m = false;
	completedJob->isFinished_m = true;

	return completedJob;
}

//----------------------------------------------------------------------

// Handles a speed-change event
bool LRPT::speedchange_handler(SpeedChangeEvent * e) {
	// PS doesn't care about the speed change.
	return false;
}

//----------------------------------------------------------------------

// Set the isFinished of job with parameter id. Returns true if the job is found.
bool LRPT::setFinish(unsigned long id, double time) {
	for (set<Job *, CompareJobLRPT>::iterator it = jobs_q.begin(); it != jobs_q.end(); it++) {
		if ((*it)->getID() == id) {
			yLog::logtime(DEBUGLOG, __FUNCTION__, "Job %d is maked finished at time %f.", id, time);
			(*it)->isFinished_m = true;
			return true;
		}
	}
	if (!jobs_q.empty())
		yLog::logtime(ERRORLOG, __FUNCTION__, "Job %d is not found to mark isFinished at time %f.", id, time);
	return false;
}

//----------------------------------------------------------------------

// Returns the job-ID of the smallest job under execution which is not marked finished
unsigned long LRPT::getExJob(double time) {
	if (jobs_q.empty()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
	} 
	else {
		for (set<Job *, CompareJobLRPT>::iterator it = jobs_q.begin(); it != jobs_q.end(); it++) {
			if (!(*it)->isFinished_m) {
				yLog::logtime(DEBUGLOG, __FUNCTION__, "Job %d is under execution and not marked isFinished at time %f.", (*it)->getID(), time);
				if ((*jobs_q.begin())->isBeingExecuted_m == false)
					yLog::logtime(ERRORLOG, __FUNCTION__, "Job %d is not marked as being executed at time %f.", (*it)->getID(), time);

				return (*it)->getID();
			}
		}
	}
	return JobID_Type::INVALIDID;
}

//----------------------------------------------------------------------

// Creates a new departure Event object and retunrs its pointer
Event * LRPT::nextDeparture(double speed, double time) {
	if (jobs_q.empty()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
		return NULL;
	}

	if (!approximatelyEqual(time, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time);

	if (approximatelyEqual(speed, 0)) {		
		if (!approximatelyEqual((*jobs_q.begin())->remsize_m, 0))
			yLog::logtime(ERRORLOG, __FUNCTION__, "Running at speed 0 while there is a job in the system.");
		else
			return NULL;
	}

	double deptime = time;
	int cnt = 0;	

	if (!approximatelyEqual((*jobs_q.begin())->remsize_m, 0)) {
		set<Job *, CompareJobLRPT>::iterator it = jobs_q.begin(); 
		while (it != jobs_q.end()) {
			double currSize = (*it)->remsize_m;
			double nextSize = 0;	
		
			while (it != jobs_q.end() && approximatelyEqual((*it)->remsize_m, currSize)) {
				it++;
				cnt++;
			}
		
			if (it != jobs_q.end())
				nextSize = (*it)->remsize_m;

			deptime += (currSize - nextSize) * cnt / speed;
		}
	}

	unsigned validid = nextValidDepartureID();
	Event * depevent = new DepartureEvent(deptime, Event_Type::DEPARTURE_EXPECTED, (*jobs_q.begin())->getID(), validid);
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

void LRPT::updatePeriod(double time1, double time2, double speed, DESLogger * logger, PowerFunction * P) {
	if (definitelyGreaterThan(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);
	
	if (definitelyLessThan(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal duplicate update over period [%d, %d].", time1, lastUpdate_m);

	if (jobs_q.empty()) 
		yLog::logtime(DEBUGLOG, __FUNCTION__, "No job to update in interval [%10f, %10f].", time1, time2);
	
	else if (!approximatelyEqual((*jobs_q.begin())->remsize_m, 0)) {
		vector <unsigned long> updateeID;
		vector <double> updateSpeeds;

		// Check if the update period is valid
		double deptime = 0;
		int cnt = 0;	
		set<Job *, CompareJobLRPT>::iterator it = jobs_q.begin(); 
		double currSize = (*it)->remsize_m;
		double nextSize = 0;	
		while (it != jobs_q.end() && approximatelyEqual((*it)->remsize_m, currSize)) {
			it++;
			cnt++;
		}
		if (it != jobs_q.end())
			nextSize = (*it)->remsize_m;
		deptime += (currSize - nextSize) * cnt / speed;
		if (definitelyLessThan(deptime, time2 - time1)) {
			if (approximatelyEqual(deptime, time2 - time1)) 
				yLog::logtime(ERRORLOG, __FUNCTION__, "definitelyLessThan messing up!");
			else
				yLog::logtime(ERRORLOG, __FUNCTION__, "Update period [%f, %f] overlaps with scheduling event for remsize class change.", time1, time2);
		}
		
		// Apply the update on the current class of executing jobs
		
		for (set<Job *, CompareJobLRPT>::iterator it2 = jobs_q.begin(); it2 != it; it2++) {
			Job * updatee = *it2;
			double takeout = (time2 - time1) * (speed / cnt);
		
			// Update the record for the updatee job
			if (approximatelyEqual(updatee->remsize_m - takeout, 0))
				updatee->remsize_m = 0;
			else if (definitelyGreaterThan(takeout, updatee->remsize_m)) {
				yLog::logtime(ERRORLOG, __FUNCTION__, 
					"Job %d with size %f and takeout %f finishes before the end of interval [%10f, %10f].", 
						updatee->getID(), updatee->remsize_m, takeout, time1, time2);
				updatee->remsize_m = 0;
			}
			else
				updatee->remsize_m -= takeout;

			double totalPowerConsumed = logger->powerConsumed(speed, time2 - time1, P);
			updatee->energyConsumed_m += totalPowerConsumed / cnt;// Add only the its share of energy consumption
			updatee->executiontime_m += time2 - time1;
			yLog::logtime(DEBUGLOG, __FUNCTION__, "Updating remaining work of job %d in interval [%10f, %10f].", updatee->getID(), time1, time2);
			updateeID.push_back(updatee->getID());
			updateSpeeds.push_back(speed/cnt);
		}
		if (logger != NULL)
			logger->execution_handler(updateeID, updateSpeeds, time1, time2);
	}
	lastUpdate_m = time2;
}	

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event * LRPT::bonusevent_handler(SchedulerEvent * e, double speed) {
	// Check if another scheduler event is required.
	return nextScheduler(speed, e->time);
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event * LRPT::nextScheduler(double speed, double time) {
	// Interrupt to force a call to update upon finishing processor sharing on the 
	// largest class of jobs in the queue.

	if (jobs_q.empty()) 
		return NULL;
	else if (approximatelyEqual((*jobs_q.begin())->remsize_m, 0)) 
		return NULL;
	else {
		// Find the next valid update period
		double deptime = time;
		int cnt = 0;	
		set<Job *, CompareJobLRPT>::iterator it = jobs_q.begin(); 
		double currSize = (*it)->remsize_m;
		double nextSize = 0;	
		while (it != jobs_q.end() && approximatelyEqual((*it)->remsize_m, currSize)) {
			it++;
			cnt++;
		}
		if (it != jobs_q.end())
			nextSize = (*it)->remsize_m;
		deptime += (currSize - nextSize) * cnt / speed;

		return new SchedulerEvent(deptime, 
							  Event_Type::SCHEDULER, 
							  (*jobs_q.begin())->getID(), 
							  nextValidSchedulerID(), 
							  Event_Type::SCHEDULER);
	}
	return NULL;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
