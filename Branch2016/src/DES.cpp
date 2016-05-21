/***
* DES.cpp - implementation of Discrete Event Simulator class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the Discrete Event Simulator class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include <iostream>

#include "DES.h"
#include "Event.h"
#include "yLog.h"

using namespace std;
	
//----------------------------------------------------------------------

DES::DES(Scheduler * scheduler, SpeedScaler * speedscaler, WorkloadGenerator * WLgenerator, DESLogger * logger, PowerFunction * P) 
	: scheduler_m(scheduler), speedscaler_m(speedscaler), workload_m(WLgenerator), logger_m(logger), P_m(P), 
	exEventTime(0), exRate_m(0), exJob_m(0), exRemaining_m(0), probeID_m(JobID_Type::INVALIDID) {	// Invalid probeID

		// Pass the power function to the speed scaler
		//speedscaler_m->setPowerFunction(P_m);

		// Initialize the simulation
		events_q.push(workload_m->next_arrival());

} // DES::DES()

//----------------------------------------------------------------------

DES::DES(Scheduler * scheduler, SpeedScaler * speedscaler, WorkloadGenerator * WLgenerator, DESLogger * logger, PowerFunction * P, 
		double probeSize, double probeInsertTime, unsigned long probeID) 
	: scheduler_m(scheduler), speedscaler_m(speedscaler), workload_m(WLgenerator), logger_m(logger), P_m(P), 
	exEventTime(0), exRate_m(0), exJob_m(0), exRemaining_m(0), probeID_m(probeID) {

		// Initialize the simulation
		events_q.push(workload_m->next_arrival());

		Job probe(probeInsertTime, JobID_Type::PROBEIDBASE, probeSize); 
		probe.set_probe(probeID);

		// Insert the probe
		ArrivalEvent * probeArrival = new ArrivalEvent(probeInsertTime, Event_Type::ARRIVAL, JobID_Type::PROBEIDBASE, probe, 0);
		events_q.push(probeArrival);

} // DES::DES()

//----------------------------------------------------------------------
// Free up the pointers in evet_q, call the destructor its constructor parameters
DES::~DES() {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Releasing the remaining event pointers.");
	while (!events_q.empty()) {
		delete events_q.top();
		events_q.pop();
	}

	delete scheduler_m;
	delete speedscaler_m;
	delete workload_m;
	delete P_m;
}

//----------------------------------------------------------------------

void DES::arrival_handler(Event * e) {
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Job %d arriving at time %10f", e->job_id, e->time);
	// Update the execution status up until the current event
	statusUpdate(e->time);

	// Let the scheduler and speedscaler know, log the arrival
	bool contextSwitch = scheduler_m->arrival_handler((ArrivalEvent*)e);
	bool speedChange = speedscaler_m->arrival_handler((ArrivalEvent*)e);
	logger_m->arrival_handler((ArrivalEvent*)e);
				
	// Put the next arrival on the queue
	Event * newEvent = workload_m->next_arrival();
	if (newEvent != NULL)
		events_q.push(newEvent);


	// Put the next speedchange on the queue
	newEvent = speedscaler_m->nextSpeedchange(e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);
	// Put the next SCHEDULER event from speedscaler on the queue
	newEvent = speedscaler_m->nextScheduler(speedscaler_m->getExSpeed(e->time), e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);

	// Put the next departure on the queue
	newEvent = scheduler_m->nextDeparture(speedscaler_m->getExSpeed(e->time), e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);
	// Put the next SCHEDULER event from scheduler on the queue
	newEvent = scheduler_m->nextScheduler(speedscaler_m->getExSpeed(e->time), e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);
}

//----------------------------------------------------------------------

void DES::departure_handler(Event * e) {
	// Update the execution status up until the current event
	statusUpdate(e->time);

	// Let the scheduler and speedscaler know, log the departure
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Job %d departing at time %10f", e->job_id, e->time);
	Job * completedJobReport = scheduler_m->departure_handler((DepartureEvent*)e);
	bool speedChange = speedscaler_m->departure_handler((DepartureEvent*)e);

	if (e->job_id <= (workload_m->getMaxN() + 1)) {
		// logger's departurehandler must free the job
		logger_m->departure_handler((DepartureEvent*)e, completedJobReport); 
	}
	else
	{	// ToCheck : Changed to see if it fixes E[T] for SRPT
		// Outside the first N jobs, throw it away, discardJob must free the job
		logger_m->departure_handler((DepartureEvent*)e, completedJobReport);
		logger_m->decreaseFirstNjobsLeft();
		//logger_m->discardJob(completedJobReport);
	}
		
	// Put the next speedchange on the queue
	Event * newEvent = speedscaler_m->nextSpeedchange(e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);
	// Put the next SCHEDULER event from speedscaler on the queue
	newEvent = speedscaler_m->nextScheduler(speedscaler_m->getExSpeed(e->time), e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);

	// Put the next departure on the queue
	newEvent = scheduler_m->nextDeparture(speedscaler_m->getExSpeed(e->time), e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);
	// Put the next SCHEDULER event from scheduler on the queue
	newEvent = scheduler_m->nextScheduler(speedscaler_m->getExSpeed(e->time), e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);
}

//----------------------------------------------------------------------

void DES::scheduler_handler(Event * e) {
	// Check if this is a valid scheduler event for the scheduler (valid scheduler events for scheduler have even validIDS)
	if (!scheduler_m->isOverwritten((SchedulerEvent *)e)) {
		// Update the execution status up until the current event
		statusUpdate(e->time);
		Event * newEvent = scheduler_m->bonusevent_handler((SchedulerEvent *)e, speedscaler_m->getExSpeed(e->time));
		if (newEvent != NULL)
			events_q.push(newEvent);
	}

	// Check if this is a valid scheduler event for the speedscaler (valid scheduler events for speed scaler have odd validIDS)
	if (!speedscaler_m->isOverwritten((SchedulerEvent *)e)) {		
		// Update the execution status up until the current event
		statusUpdate(e->time);
		Event * newEvent = speedscaler_m->bonusevent_handler((SchedulerEvent *)e);
		if (newEvent != NULL)
			events_q.push(newEvent);
	}
}

//----------------------------------------------------------------------

void DES::speedchange_handler(Event * e){
	// Update the execution status up until the current event
	statusUpdate(e->time);

	// Let the scheduler and speedscaler know, log the departure
	yLog::logtime(DEBUGLOG, __FUNCTION__, "Speed change at time %10f", e->time);
	bool contextSwitch = scheduler_m->speedchange_handler((SpeedChangeEvent*)e);
	bool speedChange = speedscaler_m->speedchange_handler((SpeedChangeEvent*)e);
	
	// The speed passed here is the old speed, which was used up until this speed change.
	logger_m->speedchange_handler(e->time, ((SpeedChangeEvent*)e)->newSpeed_m);

	// Put the next speedchange on the queue
	Event * newEvent = speedscaler_m->nextSpeedchange(e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);
	// Put the next SCHEDULER event from speedscaler on the queue
	newEvent = speedscaler_m->nextScheduler(speedscaler_m->getExSpeed(e->time), e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);

	// Put the next departure on the queue
	newEvent = scheduler_m->nextDeparture(speedscaler_m->getExSpeed(e->time), e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);
	// Put the next SCHEDULER event from scheduler on the queue
	newEvent = scheduler_m->nextScheduler(speedscaler_m->getExSpeed(e->time), e->time);
	if (newEvent != NULL)
		events_q.push(newEvent);
}

//----------------------------------------------------------------------

double DES::run() {

	//+ Add some output showing the start of the simulation, details on parameters
	// 

	bool stopDES = false;
	double lastNarrival = 0;

	while (!(events_q.empty() || stopDES)) {
		Event * e = events_q.top();
		events_q.pop();

		switch(e->type) {

//----------------------------------------------------------------------

			case Event_Type::ARRIVAL:
				
				arrival_handler(e);

				// Find the arrival of the maxN job
				if (e->job_id == workload_m->getMaxN() + 1) 
					lastNarrival = e->time;

				break;

//----------------------------------------------------------------------

			case Event_Type::DEPARTURE_EXPECTED:
				// Check if the departure has been overwritten
				if (scheduler_m->isOverwritten((DepartureEvent*)e)) 
					break;

				departure_handler(e);

				// On departure of a probe job, stop the simulation
				if (isProbe(e->job_id)) {
					yLog::logtime(DEBUGLOG, __FUNCTION__, "Probe departing at time %10f. Stopping the simulation.", e->time);
					stopDES = true;
					logger_m->clear(PROBEREPORTLOG);
					break;
				}
				else if (probeID_m != JobID_Type::PROBEIDBASE
							&& e->job_id <= (workload_m->getMaxN() + 1) 
							//&& logger_m->getTotalN() >= workload_m->getMaxN()) {
							&& logger_m->getfirstNjobsLeft() >= workload_m->getMaxN()) {

					// ToCheck : Changed : logger_m->getTotalN to logger_m->
					yLog::logtime(DEBUGLOG, __FUNCTION__, 
						"Jobs 2..%d left the system at time %10f. Stopping the simulation.", workload_m->getMaxN() + 1, e->time);
					stopDES = true;
					logger_m->clear(WLREPORTLOG);
				} 
				break;

//----------------------------------------------------------------------

			case Event_Type::DEPARTURE_ACTUAL:
				break;

//----------------------------------------------------------------------

			case Event_Type::SPEED_CHANGE:
				// Check if the speed change has been overwritten
				if (speedscaler_m->isOverwritten((SpeedChangeEvent*)e))
					break;
				
				speedchange_handler(e);

				break;

//----------------------- -----------------------------------------------

			case Event_Type::SCHEDULER:
				scheduler_handler(e);

				break;

//----------------------------------------------------------------------

			case Event_Type::ABORT:
				break;

//----------------------------------------------------------------------

			case Event_Type::INVALID: 
				yLog::logtime(ERRORLOG, __FUNCTION__, "INVALID event type in the scheduler queue!");
				break;
			default: 
				yLog::logtime(ERRORLOG, __FUNCTION__, "Unknown event type in the scheduler queue!");
		}

		delete e;	// Must be done with this event at this point.
	}
	return lastNarrival;
} // DES::run()

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void DES::statusUpdate(double time) {

	// Check if the update call is a duplicate : COULD AS WELL HAPPEN WHEN JOBS DEPART OR ARRIVE TOGETHER
	if (approximatelyEqual(time, exEventTime)) {
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Duplicate update at time %10f.", time);
		return;
	}

	// Check if the update call is illegal
	if (definitelyLessThan(time, exEventTime)) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal update at time %10f, while previous update was at %10f", time, exEventTime);
		return;
	}

	// Update scheduler
	scheduler_m->updatePeriod(exEventTime, time, speedscaler_m->getExSpeed(exEventTime), logger_m, P_m);
	speedscaler_m->updatePeriod(exEventTime, time, logger_m);

	exEventTime = time;
	return;
} // DES::statusUpdate

//----------------------------------------------------------------------

// Returns the Job object with the report of probe job's execution.
Job * DES::getCompletedProbe() {
	return new Job();
	//return (probeRoundLogger(logger_m))->getCompletedProbe();
}

//----------------------------------------------------------------------

bool DES::isProbe(unsigned long id) {
	return (id == JobID_Type::PROBEIDBASE);
}
	 
//----------------------------------------------------------------------
//----------------------------------------------------------------------
