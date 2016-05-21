/***
* SingleSpeed.cpp - implementation of SingleSpeed SpeedScaling class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the SingleSpeed SpeedScaling class.
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
#include "SingleSpeed.h"
#include "GlobalsAndTypes.h"

using namespace std;
	
//----------------------------------------------------------------------

// Object's name
string SingleSpeed::name_s = "SingleSpeed";

//----------------------------------------------------------------------

SingleSpeed::SingleSpeed(double speed, double base, PowerFunction * P) 
	: SpeedScaler(base, P) {
		speed_m = speed;
		jobCount_m = 0;
} 

//----------------------------------------------------------------------

SingleSpeed::~SingleSpeed() {
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string SingleSpeed::toString() {
	stringstream ss;
	ss << "(" << name_s << " speed=" << speed_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event, returns true if there is a speed change
bool SingleSpeed::arrival_handler(ArrivalEvent * e) {
	jobCount_m++;
	if (jobCount_m == 1)
		return true;	// New job arrived to an empty system, speed change
	else
		return false;	// No speed change
}

//----------------------------------------------------------------------

// Handles a departure event, returns true if there is a speed change
bool SingleSpeed::departure_handler(DepartureEvent * e) {
	if (jobCount_m == 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Job departure event at  time %10f, while jobCount is 0!", e->time);
	else {
		jobCount_m--;
		if (jobCount_m == 0)
			return true;
	}
	return false;
}

//----------------------------------------------------------------------

// Handles a speedchange event, returns true if there is a speed change
bool SingleSpeed::speedchange_handler(SpeedChangeEvent * e) {
	// Nothing to do here for the SingleSpeed.
	return false;
}

//----------------------------------------------------------------------

// Returns the execution speed at time. Parameter should match the internal time.
double SingleSpeed::getExSpeed(double time) {
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);

	if (jobCount_m == 0)
		return 0;			// The gated speed
	else
		return speed_m * baseSpeed_m;		// The constant speed
}

//----------------------------------------------------------------------

// Creates a new speed_change Event object and retunrs its pointer.
Event * SingleSpeed::nextSpeedchange(double time) {			
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);
	return NULL;
}

//----------------------------------------------------------------------

/* Applies the passage of time.
	- Parameter one (time1) should match the 
	internal value of previous update time. 
	- DESLogger * is used for simulation logs */
void SingleSpeed::updatePeriod(double time1, double time2, DESLogger * logger) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	// Nothing to do for the single speed

	lastUpdate_m = time2;
}
																	
//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event * SingleSpeed::bonusevent_handler(SchedulerEvent *) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event * SingleSpeed::nextScheduler(double speed, double time) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------