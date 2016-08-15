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
#include "PSBS.h"
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
string PSBS::name_s = "PSBS";

//----------------------------------------------------------------------

PSBS::PSBS() {
	vPS_m = new PS();
	latePS_m = new PS();
	numberOfLateJobs_m = 0;
	exJob_m = NULL;
} 

//----------------------------------------------------------------------

PSBS::~PSBS() {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Releasing the remaining job pointers.");
	while(!jobs_q.empty()) {
		delete (jobs_q.top());
		jobs_q.pop();
	}
	delete vPS_m;
	delete latePS_m;
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string PSBS::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event
bool PSBS::arrival_handler(ArrivalEvent * e) {
	Job tempjob = Job(e->job_m.getArrival(), e->job_m.getID(), e->job_m.remsize_m, e->job_m.getDeadline());
	ArrivalEvent tempevent(e->time, e->type, e->job_id, tempjob, e->validId_m);	
	e->job_m.remsize_m = e->job_m.getSize();

	vPS_m->arrival_handler(&tempevent);

	bool contextSwitch = false;

	Job * newjob = new Job(e->job_m);

	if (numberOfLateJobs_m > 0) {
	// PSBS is currenlty busy with the late jobs. no context switch
		jobs_q.push(newjob); 
	}
	else {
	// PSBS is not busy with the late jobs. there may be a context switch.
			// If the new arrival is the next departure under PS, 
			// -set isBeingExecuted
			// -put the current exJob on the jobs_q
			// -save the new job * to exJob
		if (newjob->getID() == vPS_m->getExJob(e->time)) {
			contextSwitch = true;
			newjob->isBeingExecuted_m = true;
			if (exJob_m != NULL) {
				exJob_m->isBeingExecuted_m = false;
				jobs_q.push(exJob_m);
			}
			exJob_m = newjob;
		}
		else {
			// Otherwise, newjob should go on the jobs_q
			jobs_q.push(newjob);
		}
	}

	if (exJob_m == NULL && numberOfLateJobs_m == 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "No job under execution, while there is at least one job in the system!");

	return contextSwitch;
}

//----------------------------------------------------------------------

// Handles a departure event
Job * PSBS::departure_handler(DepartureEvent * e) {
	Job * completedJob = NULL;
	
	// Case 1: there are late jobs
	if (numberOfLateJobs_m > 0) {
		completedJob = latePS_m->departure_handler(e); 	// Keep the pointer to pass to the logger, the logger should later free the job
		if (completedJob == NULL) {
			yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
					  							 the numberOfLateJobs > 0 but latePS->departure_handler returns NULL.", e->job_id);
			return NULL;
		}
		// Check if this is an illegal departure
		if (completedJob->getID() != e->job_id) {
			yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
					  							 while exJob_m is job %d.", e->job_id, exJob_m->getID());
			return NULL;
		}
		else if (!approximatelyEqual(completedJob->remsize_m, 0)) {
			yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job remaining size %10f, for job %d.",
				completedJob->remsize_m, completedJob->getID());
			//return NULL;
		}
		numberOfLateJobs_m--;
	}
	// Case 2: there are no late jobs
	else
	{
		// Check if exJob_m is NULL
		if (exJob_m == NULL) {
			if (!jobs_q.empty())
				yLog::logtime(ERRORLOG, __FUNCTION__, "No job under execution, while there is at least one job in the system!");
			else
				yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
													  													  							 while exJob_m is NULL.", e->job_id);
			return NULL;
		}
		// Check if this is an illegal departure
		if (exJob_m->getID() != e->job_id) {
			yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
												  												  							 while exJob_m is job %d.", e->job_id, exJob_m->getID());
			return NULL;
		}
		else if (!approximatelyEqual(exJob_m->remsize_m, 0)) {
			yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job remaining size %10f, for job %d.",
				exJob_m->remsize_m, exJob_m->getID());
			//return NULL;
		}

		completedJob = exJob_m;	// Keep the pointer to pass to the logger, the logger should later free the job
	}

	// At this point, it is certain that this is a valid actual departure
	// Take care of removal and all that needs to be updated and logged
	exJob_m = NULL;
	vPS_m->setFinish(completedJob->getID(), e->time);

	// Find the next job to execute
	// If there are no late jobs pick a new job based on vPS ordering, otherwise, continue working on late jobs.
	if (numberOfLateJobs_m == 0) {
		unsigned long nextJob = vPS_m->getExJob(e->time);
		if (nextJob == INVALIDID && !jobs_q.empty())
			yLog::logtime(ERRORLOG, __FUNCTION__,
				"PS getExJob returned INVALIDID, while there are %d jobs in PSBS!", jobs_q.size());

		vector<Job*> sack;	// Put removed jobs in the sack
		while (!jobs_q.empty()) {
			if (jobs_q.top()->getID() == nextJob) {
				exJob_m = jobs_q.top();
				jobs_q.pop();
				exJob_m->isBeingExecuted_m = true;
				break;
			}
			else {
				sack.push_back(jobs_q.top());
				jobs_q.pop();
			}
		}
		for (unsigned i = 0; i < sack.size(); i++)
			jobs_q.push(sack[i]);

		// Check if successful in setting the exJob
		if (exJob_m == NULL && !jobs_q.empty())
			yLog::logtime(ERRORLOG, __FUNCTION__,
				"No job in PSBS matches the next job from virtual PS: %d", nextJob);
	}
	completedJob->departure_m = e->time;
	completedJob->isBeingExecuted_m = false;
	completedJob->isFinished_m = true;
	return completedJob;
}

//----------------------------------------------------------------------

// Handles a speed-change event
bool PSBS::speedchange_handler(SpeedChangeEvent * e) {
	// PSBS doesn't care about the speed change.
	return false;
}

//----------------------------------------------------------------------

// TODO : needs to take care of latePS queue
// Returns the job-ID of the smallest job under execution
unsigned long PSBS::getExJob(double time) {
	// Check if exJob_m is NULL
	if (exJob_m == NULL) {
		if(!jobs_q.empty())
			yLog::logtime(ERRORLOG, __FUNCTION__, "No job under execution, while there is at least one job in the system!");
		else
			yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
	}
	else {
		return exJob_m->getID();
		//for (set<Job *, CompareJobPSBS>::iterator it = jobs_q.begin(); it != jobs_q.end(); it++) {
		//	if (!(*it)->isFinished_m) {
		//		yLog::logtime(DEBUGLOG, __FUNCTION__, "Job %d is under execution at time %f.", (*it)->getID(), time);
		//		if ((*jobs_q.begin())->isBeingExecuted_m == false)
		//			yLog::logtime(ERRORLOG, __FUNCTION__, "Job %d is not marked as being executed at time %f.", (*it)->getID(), time);

		//		return (*it)->getID();
		//	}
		//}
	}
	return JobID_Type::INVALIDID;
}

//----------------------------------------------------------------------

// Creates a new departure Event object and retunrs its pointer
Event * PSBS::nextDeparture(double speed, double time) {
	// Check if working on latePS jobs
	if (numberOfLateJobs_m > 0) {
		return (latePS_m->nextDeparture(speed, time));
	}
	// Check if exJob_m is NULL
	else {
		if (exJob_m == NULL) {
			if (!jobs_q.empty())
				yLog::logtime(ERRORLOG, __FUNCTION__, "No job under execution, while there is at least one job in the system!");
			else
				yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
			return NULL;
		}

		if (!approximatelyEqual(time, lastUpdate_m))
			yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time);

		if (approximatelyEqual(speed, 0)) {		// What if the remsize_m of top job is 0?? Should not happen
			if (!approximatelyEqual(exJob_m->remsize_m, 0))
				yLog::logtime(ERRORLOG, __FUNCTION__, "Running at speed 0 while there is a job in the system.");
			else
				return NULL;
		}

		double deptime = time + exJob_m->remsize_m / speed;
		unsigned validid = nextValidDepartureID();
		Event * depevent = new DepartureEvent(deptime, Event_Type::DEPARTURE_EXPECTED, exJob_m->getID(), validid);
		yLog::logtime(DEBUGLOG, __FUNCTION__, "New DepartureEvent created for job %d at time %f, valid id %d.",
			depevent->job_id, depevent->time, depevent->validId_m);
		return(depevent);
	}
}

//----------------------------------------------------------------------

/* Applies the passage of time.
	- Parameter one (time1) should match the 
	internal value of previous update time. 
	- Parameter three (speed), gives the speed 
	during interval [time1, time2]
	- DESLogger * is used for simulation logs 
	- Must call DESLogger.execution_handler   */

void PSBS::updatePeriod(double time1, double time2, double speed, DESLogger * logger, PowerFunction * P) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	// Should not pass the logger to vPS
	vPS_m->updatePeriod(time1, time2, speed, NULL, P);

	// Should pass the logger to latePS?
	latePS_m->updatePeriod(time1, time2, speed, logger, P);
	
	if (numberOfLateJobs_m == 0) {
		if (exJob_m == NULL) {
			if (!jobs_q.empty())
				yLog::logtime(ERRORLOG, __FUNCTION__, "No job under execution, while there is at least one job in the system!");
			else
				yLog::logtime(DEBUGLOG, __FUNCTION__, "There is no job in the system.");
		}
		else {
			vector <unsigned long> updateeID;
			vector <double> updateSpeeds;

			Job * updatee = exJob_m;
			double takeout = (time2 - time1) *  speed;

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

			updatee->energyConsumed_m += logger->powerConsumed(speed, time2 - time1, P);
			updatee->executiontime_m += time2 - time1;
			yLog::logtime(DEBUGLOG, __FUNCTION__, "Updating remaining work of job %d in interval [%10f, %10f].", updatee->getID(), time1, time2);
			updateeID.push_back(updatee->getID());
			updateSpeeds.push_back(speed);

			if (logger != NULL)
				logger->execution_handler(updateeID, updateSpeeds, time1, time2);
		}
	}
	lastUpdate_m = time2;
}	

//----------------------------------------------------------------------

// Need to check if there is a late job ***
// This should only care about the vPS departures
// Handles a bonus event, return true if anything to handle
Event * PSBS::bonusevent_handler(SchedulerEvent * e, double speed) {
	if (e->child_type != DEPARTURE_EXPECTED) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Can't handle event of type %d.", e->child_type);
		return NULL;
	}
	DepartureEvent * dep = new DepartureEvent(e->time, 
												Event_Type::SCHEDULER, 
												e->job_id, 
												0); 
	Job * completedJob = vPS_m->departure_handler(dep);
	delete dep;

	if (completedJob != NULL) {
		unsigned long vCompletedJob = completedJob->getID();
		Job * lateJob = NULL;

		// Is the late job the one currently running? Remove it from exJob_m
		if (exJob_m != NULL && exJob_m->getID() == vCompletedJob) {
			lateJob = exJob_m;
			exJob_m = NULL;
		}
		else {
			vector<Job*> sack;	// Put removed jobs in the sack
			while (!jobs_q.empty()) {
				if (jobs_q.top()->getID() == vCompletedJob) {
					// Remove the job from the queue.
					lateJob = jobs_q.top();
					jobs_q.pop();
					break;
				}
				else {
					sack.push_back(jobs_q.top());
					jobs_q.pop();
				}
			}
			for (unsigned i = 0; i < sack.size(); i++)
				jobs_q.push(sack[i]);
		}
		// FIXME: how is the context switch taken care of? The next departure is not put on the DES
		if (lateJob != NULL) {
			ArrivalEvent * latePSArrival = new ArrivalEvent(e->time, ARRIVAL, lateJob->getID(), *lateJob, 0);
			latePS_m->arrival_handler(latePSArrival);
			numberOfLateJobs_m++;
			delete lateJob;
			delete latePSArrival;

			if (exJob_m != NULL) {
				exJob_m->isBeingExecuted_m = false;
				jobs_q.push(exJob_m);
				exJob_m = NULL;
			}
		}

		// Check if successful in setting the exJob
		if (exJob_m == NULL && !jobs_q.empty() && numberOfLateJobs_m == 0)
			yLog::logtime(ERRORLOG, __FUNCTION__,
				"There are jobs in the PSBS queu, but nothing is being executed");

		delete completedJob;
	}
	// Put the next SCHEDULER event from scheduler on the queue
	return nextScheduler(speed, e->time);
}

//----------------------------------------------------------------------

// This should only care about the vPS departures
// Creates a new scheduler event, place holder for unknown desings
Event * PSBS::nextScheduler(double speed, double time) {
	DepartureEvent * vdeparture = (DepartureEvent *)vPS_m->nextDeparture(speed, time);
	if (vdeparture != NULL) {
		SchedulerEvent * ret = new SchedulerEvent(vdeparture->time, 
												  Event_Type::SCHEDULER, 
												  vdeparture->job_id, 
												  nextValidSchedulerID(), 
												  Event_Type::DEPARTURE_EXPECTED);
		delete vdeparture;
		return ret;
	}
	return NULL;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
