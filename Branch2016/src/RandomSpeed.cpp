/***
* RandomSpeed.cpp - implementation of RandomSpeed SpeedScaling class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: March, 2013
*
* Purpose:
*       This file contains the implementation of the RandomSpeed SpeedScaling class.
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
#include "RandomSpeed.h"
#include "PowerFunction.h"
#include "GlobalsAndTypes.h"

using namespace std;
	
//----------------------------------------------------------------------

// Object's name
string RandomSpeed::name_s = "RandomSpeed";

//----------------------------------------------------------------------

RandomSpeed::RandomSpeed(double base, PowerFunction * P) 
	: SpeedScaler(base, P) {
	jobCount_m = 0;
	speedMean_m = base;
	intervalMean_m = base;
	
	// Initialize speed and the nextSpeedChange event
	speed_m = base;
	nextSpeedChangeEvent_m = NULL;

	// Should the following be here?

	//double interval = base;
	//double newspeed = base;

	//nextSpeedChangeEvent_m = new SpeedChangeEvent(interval + 0, 
	//											  Event_Type::SPEED_CHANGE, 
	//											  0,
	//											  newspeed, // The speed passed here is the old speed, which was used up until this speed change.
	//											  nextValidSpeedChangeID());
} 

//----------------------------------------------------------------------

RandomSpeed::~RandomSpeed() {
	//delete shadow_m;
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string RandomSpeed::toString() {
	stringstream ss;
	ss << "(" << name_s << " base= " << baseSpeed_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event, returns true if there is a speed change
bool RandomSpeed::arrival_handler(ArrivalEvent * e) {
	jobCount_m++;
	if (jobCount_m == 1)
		return true;	// New job arrived to an empty system, speed change
	else
		return false;	// No speed change
}

//----------------------------------------------------------------------

// Handles a departure event, returns true if there is a speed change
bool RandomSpeed::departure_handler(DepartureEvent * e) {
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
bool RandomSpeed::speedchange_handler(SpeedChangeEvent * e) {
	if (e->validId_m != nextSpeedChangeEvent_m->validId_m)
		yLog::logtime(ERRORLOG, __FUNCTION__, "The speed change with id(%d) doesn't mach the internal spees change event with id(%d)!", 
																						e->validId_m, nextSpeedChangeEvent_m->validId_m);
	double speed = 0;
	while (speed == 0) {
		speed = ((double) rand() / (double)RAND_MAX) * 2.0;
	}
	speed_m = speed;

	//delete nextSpeedChangeEvent_m; The event queue will delete it when it is done with it!
	nextSpeedChangeEvent_m = NULL;

	return false;
}

//----------------------------------------------------------------------

// Returns the execution speed at time. Parameter should match the internal time.
double RandomSpeed::getExSpeed(double time) {
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);

	if (jobCount_m == 0)
		return 0;			// The gated speed
	else
		return speed_m;		// The current speed
}

//----------------------------------------------------------------------

// Creates a new speed_change Event object and retunrs its pointer.
Event * RandomSpeed::nextSpeedchange(double time) {			
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);

	if (nextSpeedChangeEvent_m == NULL) {
		double interval = 0.05;
		//while (interval == 0) {
			//interval = ((double) rand() / (double)RAND_MAX) * 2.0 ;
		//}
		nextSpeedChangeEvent_m = new SpeedChangeEvent(interval + time, 
													  Event_Type::SPEED_CHANGE, 
													  0,
													  speed_m, // The speed passed here is the old speed, which was used up until this speed change.
													  nextValidSpeedChangeID());
		return nextSpeedChangeEvent_m;
	}
	else
		return NULL;
}

//----------------------------------------------------------------------

/* Applies the passage of time.
	- Parameter one (time1) should match the 
	internal value of previous update time. 
	- DESLogger * is used for simulation logs */
void RandomSpeed::updatePeriod(double time1, double time2, DESLogger * logger) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	lastUpdate_m = time2;
}

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event *  RandomSpeed::bonusevent_handler(SchedulerEvent * e) {
	return NULL;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event * RandomSpeed::nextScheduler(double speed, double time) {
	return NULL;
}
																	
//----------------------------------------------------------------------