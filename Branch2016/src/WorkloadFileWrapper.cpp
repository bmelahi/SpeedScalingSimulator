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
#include "WorkLoadFileWrapper.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

//----------------------------------------------------------------------

// Object's name
string WorkloadFileWrapper::name_s = "WorkloadFileWrapper";
	
//----------------------------------------------------------------------

WorkloadFileWrapper::WorkloadFileWrapper(std::string filename, unsigned long maxN) {
	maxN_m = maxN;
	filename_m = string("..\\\\..\\\\inputs\\\\") + filename;
	wlfile_m.open(filename_m);
	if (!wlfile_m.fail())
		yLog::logtime(DEBUGLOG, __FUNCTION__, "Opened input workload file: %s", filename_m.c_str());
	else
		yLog::logtime(ERRORLOG, __FUNCTION__, "Failed to opened input workload file: %s", filename_m.c_str());
} 

//----------------------------------------------------------------------

WorkloadFileWrapper::~WorkloadFileWrapper() {
	wlfile_m.close();
} 

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string WorkloadFileWrapper::toString() {
	stringstream ss;
	ss << "(" << name_s << " file=" << filename_m << " maxN = " << maxN_m<< ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Creates a new arrival Event object and retunrs its pointer
Event * WorkloadFileWrapper::next_arrival() {
	double time, size, deadline;
	unsigned id = ++lastID_m;
	//wlfile_m >> time >> size >> deadline;
	//wlfile_m >> deadline >> time >> size;
	wlfile_m >> time >> deadline >> size;
	if (wlfile_m.eof()) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Reached end of workload file: %s", filename_m.c_str());
		return NULL;
	}
	return new ArrivalEvent(time, Event_Type::ARRIVAL, id, Job(time, id, size, deadline), 0);
}

//----------------------------------------------------------------------

// Creates a new arrival Event object for a probe job and retunrs its pointer
//Event * WorkloadFileWrapper::next_probe() {
//	double time = 1, size = 1, deadline = 0;
//
//	//+ set the time of the next 
//
//	return new ArrivalEvent(time, Event_Type::ARRIVAL, probeID_m, Job(time, probeID_m, size, deadline), 0);
//}

//----------------------------------------------------------------------
	
//// Returns true, if the input parameter is a probe job id
//bool WorkloadFileWrapper::isProbe(unsigned long id) {
//	// For now, we only have one probe, job ID: 1
//	return (id == probeID_m);
//}

//----------------------------------------------------------------------
	
//// Returns true, if the input parameter is a valid job id already produced
//bool WorkloadFileWrapper::isValidJobID(unsigned long id) {
//	return (id <= lastID_m && id >= firstID_m);
//}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------



		
