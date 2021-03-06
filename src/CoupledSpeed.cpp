/***
* CoupledSpeed.cpp - implementation of CoupledSpeed SpeedScaling class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the CoupledSpeed SpeedScaling class.
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
#include "CoupledSpeed.h"
#include "PowerFunction.h"
#include "GlobalsAndTypes.h"

using namespace std;
	
//----------------------------------------------------------------------

// Object's name
string CoupledSpeed::name_s = "CoupledSpeed";

//----------------------------------------------------------------------

CoupledSpeed::CoupledSpeed(double base, PowerFunction * P) 
	: SpeedScaler(base, P) {
	jobCount_m = 0;
} 

//----------------------------------------------------------------------

CoupledSpeed::~CoupledSpeed() {
	// Anything to do?
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string CoupledSpeed::toString() {
	stringstream ss;
	ss << "(" << name_s << " base= " << baseSpeed_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event, returns true if there is a speed change
bool CoupledSpeed::arrival_handler(ArrivalEvent * e) {
	jobCount_m++;
	return true;	// There is always a speed change upon the arrival of a new job.
}

//----------------------------------------------------------------------

// Handles a departure event, returns true if there is a speed change
bool CoupledSpeed::departure_handler(DepartureEvent * e) {
	if (jobCount_m == 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Job departure event at  time %10f, while jobCount is 0!", e->time);
	else 
		jobCount_m--;
	return true; // There is always a speed change upon the arrival of a new job.
}

//----------------------------------------------------------------------

// Handles a speedchange event, returns true if there is a speed change
bool CoupledSpeed::speedchange_handler(SpeedChangeEvent * e) {
	// Nothing to do here for the CoupledSpeed.
	return false;
}

//----------------------------------------------------------------------

// Returns the execution speed at time. Parameter should match the internal time.
double CoupledSpeed::getExSpeed(double time) {
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);

	if (jobCount_m == 0)
		return 0;			// The gated speed
	else
		return P_m->inverseFunction(jobCount_m) * baseSpeed_m;		// The constant speed
}

//----------------------------------------------------------------------

// Creates a new speed_change Event object and retunrs its pointer.
Event * CoupledSpeed::nextSpeedchange(double time) {			
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);
	return NULL;
}

//----------------------------------------------------------------------

/* Applies the passage of time.
	- Parameter one (time1) should match the 
	internal value of previous update time. 
	- DESLogger * is used for simulation logs */
void CoupledSpeed::updatePeriod(double time1, double time2, DESLogger * logger) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	lastUpdate_m = time2;

	// Log the speed changes
	logger->speedchange_handler(time2, getExSpeed(time2));
}

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event *  CoupledSpeed::bonusevent_handler(SchedulerEvent *) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event * CoupledSpeed::nextScheduler(double speed, double time) {
	// Anything to do?
	return NULL;
}
																	
//----------------------------------------------------------------------