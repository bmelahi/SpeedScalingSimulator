/***
* WorkloadFileWrapper.cpp - implementation of WorkloadFileWrapper WorkLoadGenerator class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the WorkloadFileWrapper WorkLoadGenerator class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "yLog.h"
#include "Event.h"
#include "GlobalsAndTypes.h"
#include "WLGenParetoEstimates.h"
#include "PRandomGen.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

//----------------------------------------------------------------------

// Object's name
string WLGenParetoEstimates::name_s = "WLGenParetoEstimates";
	
//----------------------------------------------------------------------

WLGenParetoEstimates::WLGenParetoEstimates(double arrivalRate, double alpha, unsigned long maxN) 
	: rateArr_m(arrivalRate), alpha_m(alpha), lastTime_m(0) {
	maxN_m = maxN;
	if (alpha != 1)
		load_m = rateArr_m * (alpha_m / (alpha_m - 1));

	PRandomGen::reset();

	char filename [200];
	sprintf(filename, "WLPTR--ArrRate %f -Alpha %f.dat", rateArr_m, alpha_m);

	if (rateArr_m <= 0 || alpha_m <= 1 || load_m <= 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal initialization with zero or negative values! %s", filename);
} 

//----------------------------------------------------------------------

WLGenParetoEstimates::~WLGenParetoEstimates() {
	// Anything to do?
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string WLGenParetoEstimates::toString() {
	stringstream ss;
	ss << "(" << name_s << " ArrRate=" << rateArr_m << " Alpha= " << alpha_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Creates a new arrival Event object and retunrs its pointer
Event * WLGenParetoEstimates::next_arrival() {
	double deadline = 0;
	unsigned id = ++lastID_m;

	double time = lastTime_m;
	double size = PRandomGen::getPareto(alpha_m);
	lastTime_m += PRandomGen::getExponential(1/rateArr_m);

	// FIXME : size estimation
	double estimated = size;
	if (flipSign == true) {
		estimated *= 0.5;
		flipSign = false;
	}
	else {
		estimated *= 1.5;
		flipSign = true;
	}

	// END FIXME

	return new ArrivalEvent(time, Event_Type::ARRIVAL, id, Job(time, id, size, deadline, estimated), 0);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------



		
