#pragma warning( disable : 4996 )

/***
* SEH.cpp - implementation of SEH Scheduler class
*
*		Written by Mithun P
*		Last Updated: July 2022
* 
* Purpose:
*       This file contains the implementation of the SEH Scheduler class.
*
***/

//----------------------------------------------------------------------
#include "Job.h"
#include "SEH.h"
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
string SEH::name_s = "SEH";

//----------------------------------------------------------------------

SEH::SEH() {
}

//----------------------------------------------------------------------

SEH::~SEH() {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Releasing the remaining job pointers.");
	while (!jobs_q.empty()) {
		delete jobs_q.top();
		jobs_q.pop();
	}
} // SEH::~SEH()

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string SEH::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

// Handles an arrival event
bool SEH::arrival_handler(ArrivalEvent* e) {
	Job* top = NULL;
	if (!jobs_q.empty()) {
		top = jobs_q.top();
		// Calculate index score of the top job after checking if there is underestimation.
		if (top->remsize_m > 0 && top->estimatedremsize_m <= 0) {
			top->sehIndexScore_m = 2 / (top->getEstimatedSize());
		}
		else {
			top->sehIndexScore_m = 1 / (top->getEstimatedSize() - top->executiontime_m * (1 - top->executiontime_m / (2 * top->getEstimatedSize())));
		}
	}
	
	/*
		if(top->isUnderestimated_m == false){
			if (top->remsize_m >0 && top->estimatedremsize_m<=0) {
				top->isUnderestimated_m == true
				top->sehIndexScore_m = 2 / (top->getEstimatedSize());
			}
			else{
			top->sehIndexScore_m = 1 / (top->getEstimatedSize() -
				top->executiontime_m * (1 - top->executiontime_m / (2 * top->getEstimatedSize())));
			}
		}
	*/

	Job* newjob = new Job(e->job_m);

	// Calculate index score of the newly arrived job
	// Since newly arrived job's execution time is 0, no need to check for underestimation
	newjob->sehIndexScore_m = 1 / (newjob->getEstimatedSize() - newjob->executiontime_m * (1 - newjob->executiontime_m / (2 * newjob->getEstimatedSize())));
	jobs_q.push(newjob);


	// If the new job is on top of SEH queue, the there has been a context switch
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

// Handles a departure event
Job* SEH::departure_handler(DepartureEvent* e) {
	//yLog::logtime(DEBUGLOG, __FUNCTION__, "");
	// Check if queue is empty
	if (jobs_q.empty()) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
											  						 while queue is empty.", e->job_id);
		return NULL;
	}
	// Check if this is an illegal departure
	if ((jobs_q.top())->getID() != e->job_id) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
											  						 while top of queue is job %d.", e->job_id, (jobs_q.top())->getID());
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

	completedJob->departure_m = e->time;
	completedJob->isBeingExecuted_m = false;
	completedJob->isFinished_m = true;
	return completedJob;
}

// Handles a speed-change event
bool SEH::speedchange_handler(SpeedChangeEvent* e) {
	return false;
}


// Returns the job-ID of the current job under execution
unsigned long SEH::getExJob(double time) {
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

// Creates a new departure Event object and retunrs its pointer
Event* SEH::nextDeparture(double speed, double time) {
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

// Handles a bonus event, return true if anything to handle
Event* SEH::bonusevent_handler(SchedulerEvent*, double speed) {
	// Anything to do?
	return NULL;
}

// Creates a new scheduler event, place holder for unknown desings
Event* SEH::nextScheduler(double speed, double time) {
	return NULL;
}

//----------------------------------------------------------------------
	// Check if Underestimation has occured.
	// If true
	// Then the index score = 2/s (2/estimated_size)
	// Else
	// index score =1/(s-a(1-a/(2*s)))
	// where a is the elapsed time and s is the estimated job processesing time

void SEH::updatePeriod(double time1, double time2, double speed, DESLogger* logger, PowerFunction* P) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	if (jobs_q.empty())
		yLog::logtime(DEBUGLOG, __FUNCTION__, "No job to update in interval [%10f, %10f].", time1, time2);
	else {
		Job* updatee = jobs_q.top();
		double takeout = (time2 - time1) * speed;

		if (approximatelyEqual(updatee->remsize_m - takeout, 0)) {
			updatee->remsize_m = 0;
		}
		else if (definitelyGreaterThan(takeout, updatee->remsize_m)) {
			yLog::logtime(ERRORLOG, __FUNCTION__,
				"Job %d with size %f and takeout %f finishes before the end of interval [%10f, %10f].",
				updatee->getID(), updatee->remsize_m, takeout, time1, time2);
			updatee->remsize_m = 0;
		}
		else {
			updatee->remsize_m -= takeout;
			updatee->estimatedremsize_m -= takeout;
		}

		// Check for underestimation and calculate Index Score
		if (updatee->remsize_m > 0 && updatee->estimatedremsize_m <= 0) {
			updatee->sehIndexScore_m = 2 / (updatee->getEstimatedSize());
		}
		else {
			updatee->sehIndexScore_m = 1 / (updatee->getEstimatedSize() -
				updatee->executiontime_m * (1 - updatee->executiontime_m / (2 * updatee->getEstimatedSize())));
		}

		/*
		if(updatee->isUnderestimated_m == false){
			if (updatee->remsize_m>0 && updatee->estimatedremsize_m<=0) {
				updatee->isUnderestimated_m == true;
				updatee->sehIndexScore_m = 2 / (updatee->getEstimatedSize());
			}
			else{
			updatee->sehIndexScore_m = 1 / (updatee->getEstimatedSize() -
				updatee->executiontime_m * (1 - updatee->executiontime_m / (2 * updatee->getEstimatedSize())));
			}
		}
		*/

		updatee->energyConsumed_m += logger->powerConsumed(speed, time2 - time1, P);
		updatee->executiontime_m += time2 - time1;
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Updating remaining work of job %d in interval [%10f, %10f].", updatee->getID(), time1, time2);
		if (logger != NULL)
			logger->execution_handler(vector<unsigned long>(1, updatee->getID()), vector<double>(1, speed), time1, time2);
	}
	lastUpdate_m = time2;

}