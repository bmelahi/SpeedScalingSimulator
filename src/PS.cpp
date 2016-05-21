/***
* SRPT.cpp - implementation of SRPT Scheduler class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the SRPT Scheduler class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "PS.h"
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
string PS::name_s = "PS";

//----------------------------------------------------------------------

PS::PS() {
		// Anything to do?
} 

//----------------------------------------------------------------------

PS::~PS() {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Releasing the remaining job pointers.");
	while(!jobs_q.empty()) {
		delete (*jobs_q.begin());
		jobs_q.erase(jobs_q.begin());
	}
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string PS::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event
bool PS::arrival_handler(ArrivalEvent * e) {
	Job * newjob = new Job (e->job_m);
	newjob->isBeingExecuted_m = true;
	jobs_q.insert(newjob);

	//// FIXME
	//stringstream ss;
	//ss.precision(10);
	//ss.setf(std::ios::fixed, std::ios::floatfield);

	//double touched = 0;
	//double untouched = newjob->getSize();
	//int tcnt = jobs_q.size() - 1;
	//int ucnt = 1;
	//string contextswitch = "A";

	//ss << newjob->getSize();
	//for (set<Job*, CompareJobPS>::iterator it = jobs_q.begin(); it != jobs_q.end(); it++) {
	//	Job * updatee = *it;
	//	if (newjob->getID() != updatee->getID()) {
	//		ss << ' ';
	//		touched += updatee->remsize_m;
	//		ss << updatee->remsize_m;
	//	}
	//}

	//ss << " (" << newjob->getSize() << ')';

	//yLog::log(QUEUEDUMP, "%10.10f %s %10.10f %10.10f %5d %5d qd: %s", 
	//	e->time, 
	//	contextswitch.c_str(), 
	//	touched, 
	//	untouched, 
	//	tcnt,
	//	ucnt,
	//	ss.str().c_str());
	 //END FIXME


	return true;
}

//----------------------------------------------------------------------

// Handles a departure event
Job * PS::departure_handler(DepartureEvent * e) {
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

	// At this point, it is certain that this is a valid actual departure
	// Take care of removal and all that needs to be updated and logged
	Job * completedJob = (*jobs_q.begin());	// Keep the pointer to pass to the logger, the logger should later free the job
	jobs_q.erase(jobs_q.begin());

	// FIXME
	//if (jobs_q.empty()) {
	//	double empty = 0;
	//	yLog::log(QUEUEDUMP, "%10.10f d %10.10f %10.10f %5d %5d qd:", e->time, empty, empty, empty, empty);
	//}
	//else { //if (!jobs_q.empty())
	//	stringstream ss;
	//	ss.precision(10);
	//	ss.setf(std::ios::fixed, std::ios::floatfield);

	//	double touched = 0;
	//	double untouched = 0;
	//	int tcnt = jobs_q.size();
	//	int ucnt = 0;

	//	for (set<Job*, CompareJobPS>::iterator it = jobs_q.begin(); it != jobs_q.end(); it++) {
	//		Job * updatee = *it;
	//		if (it != jobs_q.begin())
	//			ss << ' ';
	//		touched += updatee->remsize_m;
	//		ss << updatee->remsize_m;
	//	}

	//	yLog::log(QUEUEDUMP, "%10.10f d %10.10f %10.10f %5d %5d qd: %s",
	//		e->time,
	//		touched,
	//		untouched,
	//		tcnt,
	//		ucnt,
	//		ss.str().c_str());
	//}
	//END FIXME

	completedJob->departure_m = e->time;
	completedJob->isBeingExecuted_m = false;
	completedJob->isFinished_m = true;
	return completedJob;
}

//----------------------------------------------------------------------

// Handles a speed-change event
bool PS::speedchange_handler(SpeedChangeEvent * e) {
	// PS doesn't care about the speed change.
	return false;
}

//----------------------------------------------------------------------

// Set the isFinished of job with parameter id. Returns true if the job is found.
bool PS::setFinish(unsigned long id, double time) {
	for (set<Job *, CompareJobPS>::iterator it = jobs_q.begin(); it != jobs_q.end(); it++) {
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
unsigned long PS::getExJob(double time) {
	if (jobs_q.empty()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
	} 
	else {
		for (set<Job *, CompareJobPS>::iterator it = jobs_q.begin(); it != jobs_q.end(); it++) {
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
Event * PS::nextDeparture(double speed, double time) {
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

	double deptime = time + (*jobs_q.begin())->remsize_m / (speed / jobs_q.size());
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

void PS::updatePeriod(double time1, double time2, double speed, DESLogger * logger, PowerFunction * P) {
	if (definitelyGreaterThan(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);
	
	if (definitelyLessThan(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal duplicate update over period [%d, %d].", time1, lastUpdate_m);

	if (jobs_q.empty()) 
		yLog::logtime(DEBUGLOG, __FUNCTION__, "No job to update in interval [%10f, %10f].", time1, time2);
	else {
		vector <unsigned long> updateeID;
		vector <double> updateSpeeds;

		for (set<Job*, CompareJobPS>::iterator it = jobs_q.begin(); it != jobs_q.end(); it++) {
			Job * updatee = *it;
			double takeout = (time2 - time1) * (speed / jobs_q.size());
		
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
			updatee->energyConsumed_m += totalPowerConsumed / jobs_q.size();// Add only the its share of energy consumption
			updatee->executiontime_m += time2 - time1;
			yLog::logtime(DEBUGLOG, __FUNCTION__, "Updating remaining work of job %d in interval [%10f, %10f].", updatee->getID(), time1, time2);
			updateeID.push_back(updatee->getID());
			updateSpeeds.push_back(speed/jobs_q.size());
		}
		if (logger != NULL)
			logger->execution_handler(updateeID, updateSpeeds, time1, time2);
	}
	lastUpdate_m = time2;
}	

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event * PS::bonusevent_handler(SchedulerEvent *, double speed) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event * PS::nextScheduler(double speed, double time) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
