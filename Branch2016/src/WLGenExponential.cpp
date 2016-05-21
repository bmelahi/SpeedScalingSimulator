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
#include "WLGenExponential.h"
#include "PRandomGen.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

//----------------------------------------------------------------------

// Object's name
string WLGenExponential::name_s = "WLGenExponential";
	
//----------------------------------------------------------------------

WLGenExponential::WLGenExponential(double arrivalRate, double meanSize, unsigned long maxN) 
	: rateArr_m(arrivalRate), meanSize_m(meanSize), lastTime_m(0) {
	maxN_m = maxN;
	load_m = rateArr_m * meanSize_m;

	PRandomGen::reset();
	//PRandomGen::setupGen("..\\\\..\\\\inputs\\\\100MBRandomBytes-29NOV2012.dat");

	char filename [200];
	sprintf(filename, "WLExp--ArrRate %f -meanSize %f.dat", rateArr_m, meanSize_m);

	if (rateArr_m <= 0 || meanSize <= 0 || load_m <= 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal initialization with zero or negative values! %s", filename);

	WLG_s.open(filename);
} 

//----------------------------------------------------------------------

WLGenExponential::~WLGenExponential() {
	// Anything to do?
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string WLGenExponential::toString() {
	stringstream ss;
	ss << "(" << name_s << " ArrRate=" << rateArr_m << " meanSize= " << meanSize_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Creates a new arrival Event object and retunrs its pointer
Event * WLGenExponential::next_arrival() {
	double deadline = 0;
	unsigned id = ++lastID_m;

	double time = lastTime_m;
	double size = PRandomGen::getExponential(meanSize_m);
	lastTime_m += PRandomGen::getExponential(1/rateArr_m);

	char line[500];
	sprintf(line, "%10.10f %10.10f %d", time, size, id);
	//WLG_s << time << "\t" << size << "\t" << id << endl;
	WLG_s << line << endl;
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



		
