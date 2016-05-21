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
#include "WLGenBatchExponential.h"
#include "PRandomGen.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

//----------------------------------------------------------------------

// Object's name
string WLGenBatchExponential::name_s = "WLGenBatchExponential";
	
//----------------------------------------------------------------------

WLGenBatchExponential::WLGenBatchExponential(double meanBatchCount, double meanBatchInterval, double meanSize, unsigned long maxN) 
	: meanBatchCount_m(meanBatchCount), meanBatchInterval_m(meanBatchInterval), meanSize_m(meanSize), 
	  lastTime_m(0), lastCount_m(0), currentRandomCount_m(0) {
		  
	maxN_m = maxN;
	load_m = meanBatchCount_m * meanSize_m * meanBatchInterval_m;

	PRandomGen::reset();

	currentRandomCount_m = PRandomGen::getExponential(meanBatchCount_m);
	
	char filename [200];
	sprintf(filename, "WLExp--BatchCount %f-BatchInterval %f-meanSize %f.dat", meanBatchCount_m, meanBatchInterval_m, meanSize_m);

	if (meanBatchCount <= 0 || meanBatchInterval_m <= 0 || meanSize <= 0 || load_m <= 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal initialization with zero or negative values! %s", filename);
} 

//----------------------------------------------------------------------

WLGenBatchExponential::~WLGenBatchExponential() {
	// Anything to do?
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string WLGenBatchExponential::toString() {
	stringstream ss;
	ss << "(" << name_s << " BatchCount=" << meanBatchCount_m << " BatchInterval" << meanBatchInterval_m << " meanSize= " << meanSize_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Creates a new arrival Event object and retunrs its pointer
Event * WLGenBatchExponential::next_arrival() {
	double deadline = 0;
	unsigned id = ++lastID_m;

	double time = lastTime_m;
	double size = PRandomGen::getExponential(meanSize_m);

	if (lastCount_m < currentRandomCount_m) {
		lastCount_m++;

	}
	else {
		lastTime_m += PRandomGen::getExponential(meanBatchInterval_m);
		currentRandomCount_m = PRandomGen::getExponential(meanBatchCount_m);
		lastCount_m = 0;
	}

	return new ArrivalEvent(time, Event_Type::ARRIVAL, id, Job(time, id, size, deadline), 0);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------



		
