/***
* WLGenCustom.cpp - implementation of WLGenCustom class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: March, 2013
*
* Purpose:
*       This file contains the implementation of the WLGenCustom class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "yLog.h"
#include "Event.h"
#include "GlobalsAndTypes.h"
#include "WLGenCustom.h"
#include "PRandomGen.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

//----------------------------------------------------------------------

// Object's name
string WLGenCustom::name_s = "WLGenCustom";
	
//----------------------------------------------------------------------

WLGenCustom::WLGenCustom(double arrivalRate, double meanSize, unsigned long maxN) 
	: rateArr_m(arrivalRate), meanSize_m(meanSize), lastTime_m(0) {
	maxN_m = maxN;
	load_m = rateArr_m * meanSize_m;

	PRandomGen::reset();

	char filename [200];
	sprintf(filename, "WLCustom--ArrRate %f -meanSize %f.dat", rateArr_m, meanSize_m);

	if (rateArr_m <= 0 || meanSize <= 0 || load_m <= 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal initialization with zero or negative values! %s", filename);
} 

//----------------------------------------------------------------------

WLGenCustom::~WLGenCustom() {
	// Anything to do?
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string WLGenCustom::toString() {
	stringstream ss;
	ss << "(" << name_s << " ArrRate=" << rateArr_m << " meanSize= " << meanSize_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Creates a new arrival Event object and retunrs its pointer
Event * WLGenCustom::next_arrival() {
	double deadline = 0;
	unsigned id = ++lastID_m;

	double time = lastTime_m;
	double size = (id % (int)meanSize_m == 0)? 2 : 0.8;
	lastTime_m += 1/rateArr_m;

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



		
