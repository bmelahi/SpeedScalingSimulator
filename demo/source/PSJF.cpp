/***
* PSJF.cpp - implementation of PSJF Scheduler class
*
*       Tutorial example: adding a new scheduling policy to S3.
*
* Purpose:
*       Preemptive Shortest Job First - see PSJF.h. Every method below is
*       the standard Scheduler boilerplate; the only part that encodes the
*       POLICY is CompareJobPSJF in the header, plus the preemption check
*       in arrival_handler().
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "PSJF.h"
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
string PSJF::name_s = "PSJF";

//----------------------------------------------------------------------

PSJF::PSJF() {
} // PSJF::PSJF()

//----------------------------------------------------------------------

PSJF::~PSJF() {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Releasing the remaining job pointers.");
	while (!jobs_q.empty()) {
		delete jobs_q.top();
		jobs_q.pop();
	}
} // PSJF::~PSJF()

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string PSJF::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event
bool PSJF::arrival_handler(ArrivalEvent * e) {
	Job * top = NULL;
	if (!jobs_q.empty())
		top = jobs_q.top();

	Job * newjob = new Job (e->job_m);
	jobs_q.push(newjob);

	// If the new job is now on top of the queue, it has preempted whatever
	// was running. This is the whole of the "preemptive" in PSJF: the queue
	// ordering decides, and we simply notice when the top changed.
	if (jobs_q.top()->getID() == newjob->getID()) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Context switch, job %d preempts job %d.",
											newjob->getID(), (top == NULL) ? 0 : (top->getID()));
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
Job * PSJF::departure_handler(DepartureEvent * e) {
	if (jobs_q.empty()) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
											  						 while queue is empty.", e->job_id);
		return NULL;
	}
	if ((jobs_q.top())->getID() != e->job_id) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job %d, \
											  						 while top of queue is job %d.", e->job_id, (jobs_q.top())->getID());
		return NULL;
	}
	else if (!approximatelyEqual(jobs_q.top()->remsize_m, 0)) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal departure handler call for job remaining size %10f, for job %d.",
			jobs_q.top()->remsize_m, jobs_q.top()->getID());
	}

	Job * completedJob = jobs_q.top();	// Logger takes ownership and frees this later
	jobs_q.pop();

	if (!jobs_q.empty())
		jobs_q.top()->isBeingExecuted_m = true;

	completedJob->departure_m = e->time;
	completedJob->isBeingExecuted_m = false;
	completedJob->isFinished_m = true;
	return completedJob;
}

//----------------------------------------------------------------------

// Handles a speed-change event
bool PSJF::speedchange_handler(SpeedChangeEvent * e) {
	// PSJF's ordering does not depend on speed, so nothing to do.
	return false;
}

//----------------------------------------------------------------------

// Returns the job-ID of the current job under execution
unsigned long PSJF::getExJob(double time) {
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

// Creates a new departure Event object and returns its pointer
Event * PSJF::nextDeparture(double speed, double time) {
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
	Event * depevent = new DepartureEvent(deptime, Event_Type::DEPARTURE_EXPECTED, jobs_q.top()->getID(), validid);
	yLog::logtime(DEBUGLOG, __FUNCTION__, "New DepartureEvent created for job %d at time %f, valid id %d.",
														depevent->job_id, depevent->time, depevent->validId_m);
	return(depevent);
}

//----------------------------------------------------------------------

/* Applies the passage of time.
	- Parameter one (time1) should match the internal value of previous update time.
	- Parameter three (speed) gives the speed during interval [time1, time2]
	- DESLogger * is used for simulation logs
	- Must call DESLogger.execution_handler   */

void PSJF::updatePeriod(double time1, double time2, double speed, DESLogger * logger, PowerFunction * P) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	if (jobs_q.empty())
		yLog::logtime(DEBUGLOG, __FUNCTION__, "No job to update in interval [%10f, %10f].", time1, time2);
	else {
		Job * updatee = jobs_q.top();
		double takeout = (time2 - time1) * speed;

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
		if (logger != NULL)
			logger->execution_handler(vector<unsigned long> (1, updatee->getID()), vector<double> (1, speed), time1, time2);
	}
	lastUpdate_m = time2;
}

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event * PSJF::bonusevent_handler(SchedulerEvent *, double speed) {
	return NULL;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown designs
Event * PSJF::nextScheduler(double speed, double time) {
	return NULL;
}

//----------------------------------------------------------------------
