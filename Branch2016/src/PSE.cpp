/***
* PSE.cpp - implementation of PSE Scheduler class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the PSE Scheduler class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "PSE.h"
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
string PSE::name_s = "PSE";

//----------------------------------------------------------------------

PSE::PSE() {
		// Anything to do?
} 

//----------------------------------------------------------------------

PSE::~PSE() {
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string PSE::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

void PSE::clearQueue(double time) {		// Clear the remaining work in the queue update the time and the validID.
	lastUpdate_m = time;
	nextValidDepartureID();
	nextValidSchedulerID();
	jobs_q.clear();
}

//----------------------------------------------------------------------