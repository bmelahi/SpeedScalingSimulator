/***
* ShadowSpeed.cpp - implementation of ShadowSpeed SpeedScaling class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the ShadowSpeed SpeedScaling class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include <cmath> 
#include <string>
#include <sstream>

#include "Job.h"
#include "yLog.h"
#include "Event.h"
#include "ShadowSpeed.h"
#include "PowerFunction.h"
#include "GlobalsAndTypes.h"

using namespace std;
	
//----------------------------------------------------------------------

// Object's name
string ShadowSpeed::name_s = "ShadowSpeed";

//----------------------------------------------------------------------

ShadowSpeed::ShadowSpeed(double base, PowerFunction * P, Scheduler * shadow) 
	: SpeedScaler(base, P) {
	shadow_m = shadow;
	jobCount_m = 0;
	shadowJobCount_m = 0;
} 

//----------------------------------------------------------------------

ShadowSpeed::~ShadowSpeed() {
	delete shadow_m;
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string ShadowSpeed::toString() {
	stringstream ss;
	ss << "(" << name_s << " base= " << baseSpeed_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event, returns true if there is a speed change
bool ShadowSpeed::arrival_handler(ArrivalEvent * e) {
	shadow_m->arrival_handler(e);
	jobCount_m++;
	shadowJobCount_m++;
	//speed = shadow_m->
	return true;	// There is always a speed change upon the arrival of a new job.
}

//----------------------------------------------------------------------

// Handles a departure event, returns true if there is a speed change
bool ShadowSpeed::departure_handler(DepartureEvent * e) {
	if (jobCount_m == 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Job departure event at  time %10f, while jobCount is 0!", e->time);
	else 
		jobCount_m--;

	return false;	// ShadowSpeed is insensitive to scheduler departures
}

//----------------------------------------------------------------------

// Handles a speedchange event, returns true if there is a speed change
bool ShadowSpeed::speedchange_handler(SpeedChangeEvent * e) {
	if (shadowJobCount_m == 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Job departure event at  time %10f, while jobCount is 0!", e->time);
	else 
		shadowJobCount_m--;

	DepartureEvent * dep = new DepartureEvent(e->time, 
											  Event_Type::SPEED_CHANGE, 
											  e->job_id, 
											  0); 
	Job * completedJob = shadow_m->departure_handler(dep);
	delete dep;
	if (completedJob != NULL) {
		delete completedJob;
		return true; // There is always a speed change upon the completion a new job.
	}
	else {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Unsuccessful call to the shadow departure handler at time %f", e->time);
		return false;
	}
}

//----------------------------------------------------------------------

// Returns the execution speed at time. Parameter should match the internal time.
double ShadowSpeed::getExSpeed(double time) {
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);

	if (jobCount_m == 0)
		return 0;			// The gated speed
	else
		return P_m->inverseFunction(shadowJobCount_m) * baseSpeed_m;		// The constant speed
}

//----------------------------------------------------------------------

// Creates a new speed_change Event object and retunrs its pointer.
Event * ShadowSpeed::nextSpeedchange(double time) {			
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);

	double speed = getExSpeed(time);
	DepartureEvent * shadowDeparture = (DepartureEvent *)shadow_m->nextDeparture(speed, time);
	if (shadowDeparture != NULL) {
		SpeedChangeEvent * ret = new SpeedChangeEvent(shadowDeparture->time, 
													  Event_Type::SPEED_CHANGE, 
													  shadowDeparture->job_id,
													  speed, // The speed passed here is the old speed, which was used up until this speed change.
													  nextValidSpeedChangeID());
		delete shadowDeparture;
		return ret;
	}
	return NULL;
}

//----------------------------------------------------------------------

/* Applies the passage of time.
	- Parameter one (time1) should match the 
	internal value of previous update time. 
	- DESLogger * is used for simulation logs */
void ShadowSpeed::updatePeriod(double time1, double time2, DESLogger * logger) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	shadow_m->updatePeriod(time1, time2, getExSpeed(time1), NULL, P_m);
	lastUpdate_m = time2;
}

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event *  ShadowSpeed::bonusevent_handler(SchedulerEvent * e) {
	SchedulerEvent * shadowScheduler = (SchedulerEvent *)shadow_m->bonusevent_handler(e, getExSpeed(e->time));
	if (shadowScheduler != NULL)
		shadowScheduler->validId_m = nextValidSchedulerID();
	return shadowScheduler;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event * ShadowSpeed::nextScheduler(double speed, double time) {
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);

	speed = getExSpeed(time);
	SchedulerEvent * shadowScheduler = (SchedulerEvent *)shadow_m->nextScheduler(speed, time);
	if (shadowScheduler != NULL) {
		SchedulerEvent * ret = new	SchedulerEvent(shadowScheduler->time, 
													  Event_Type::SCHEDULER, 
													  shadowScheduler->job_id,
													  nextValidSchedulerID(),
													  Event_Type::DEPARTURE_EXPECTED);
		delete shadowScheduler;
		return ret;
	}
	return NULL;
}
																	
//----------------------------------------------------------------------