/***
* CoupledSpeedWith1Addition.cpp - implementation of CoupledSpeed SpeedScaling class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: July, 2017
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
#include "CoupledSpeedWith1Addition.h"
#include "PowerFunction.h"
#include "GlobalsAndTypes.h"

using namespace std;
	
//----------------------------------------------------------------------

// Object's name
string CoupledSpeedWith1Addition::name_s = "CoupledSpeedWith1Addition";

//----------------------------------------------------------------------

CoupledSpeedWith1Addition::CoupledSpeedWith1Addition(double base, PowerFunction * P)
	: SpeedScaler(base, P) {
	jobCount_m = 0;
} 

//----------------------------------------------------------------------

CoupledSpeedWith1Addition::~CoupledSpeedWith1Addition() {
	// Anything to do?
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string CoupledSpeedWith1Addition::toString() {
	stringstream ss;
	ss << "(" << name_s << " base= " << baseSpeed_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event, returns true if there is a speed change
bool CoupledSpeedWith1Addition::arrival_handler(ArrivalEvent * e) {
	jobCount_m++;
	return true;	// There is always a speed change upon the arrival of a new job.
}

//----------------------------------------------------------------------

// Handles a departure event, returns true if there is a speed change
bool CoupledSpeedWith1Addition::departure_handler(DepartureEvent * e) {
	if (jobCount_m == 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Job departure event at  time %10f, while jobCount is 0!", e->time);
	else 
		jobCount_m--;
	return true; // There is always a speed change upon the arrival of a new job.
}

//----------------------------------------------------------------------

// Handles a speedchange event, returns true if there is a speed change
bool CoupledSpeedWith1Addition::speedchange_handler(SpeedChangeEvent * e) {
	// Nothing to do here for the CoupledSpeed.
	return false;
}

//----------------------------------------------------------------------

// Returns the execution speed at time. Parameter should match the internal time.
double CoupledSpeedWith1Addition::getExSpeed(double time) {
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);

	if (jobCount_m == 0)
		return 1;			// Speed when system is empty of all work except for the permenant resident (benevolent)
	else
		return P_m->inverseFunction(jobCount_m + 1) * baseSpeed_m;		// Speed assuming the presence of the permenant resident (benevolent).
}

//----------------------------------------------------------------------

// Creates a new speed_change Event object and retunrs its pointer.
Event * CoupledSpeedWith1Addition::nextSpeedchange(double time) {
	if (!approximatelyEqual(lastUpdate_m, time))
		yLog::logtime(ERRORLOG, __FUNCTION__, "The last update time %10f does not match the input time %10f!", lastUpdate_m, time);
	return NULL;
}

//----------------------------------------------------------------------

/* Applies the passage of time.
	- Parameter one (time1) should match the 
	internal value of previous update time. 
	- DESLogger * is used for simulation logs */
void CoupledSpeedWith1Addition::updatePeriod(double time1, double time2, DESLogger * logger) {
	if (!approximatelyEqual(time1, lastUpdate_m))
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", lastUpdate_m, time1);

	lastUpdate_m = time2;

	// Log the speed changes
	logger->speedchange_handler(time2, getExSpeed(time2));
}

//----------------------------------------------------------------------

// Handles a bonus event, return true if anything to handle
Event *  CoupledSpeedWith1Addition::bonusevent_handler(SchedulerEvent *) {
	// Anything to do?
	return NULL;
}

//----------------------------------------------------------------------

// Creates a new scheduler event, place holder for unknown desings
Event * CoupledSpeedWith1Addition::nextScheduler(double speed, double time) {
	// Anything to do?
	return NULL;
}
																	
//----------------------------------------------------------------------