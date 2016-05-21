/***
* RoundZeroLogger.cpp - implementation of RoundZeroLogger class (of DESLogger)
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the RoundZeroLogger class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "yLog.h"
#include "Event.h"
#include "RoundZeroLogger.h"
#include "GlobalsAndTypes.h"

#include <ctime>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <Windows.h>

using namespace std;

//----------------------------------------------------------------------

// Object's name
string RoundZeroLogger::name_s = "RoundZeroLogger";

//----------------------------------------------------------------------

RoundZeroLogger::RoundZeroLogger(string debugmode, std::string writemode) 
	: prevSpeed_m(0), prevTime_m (0), byteCount_m(0), prevExecutionTime_m(0), zeroRemaining_m(true) {

	inDebugMode_m = (debugmode == "VERBOSE")? true : false;
	inPromptMode_m = (writemode == "PROMTWRITE")? true : false;

	// Set the loggers based on the mode 
	//if (inDebugMode_m) {
		yLog::setLogFile("workloadreport.txt", WLREPORTLOG);
		yLog::setLogFile("workloadbyteprofile.txt", BYTELOG);
		yLog::setLogFile("speedprofile.txt", SPEEDLOG);
		yLog::setLogFile("jobcountprofile.txt", JOBCOUNTLOG);
	//}

	yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "----------------------------RoundZeroLogger");

	yLog::logtime(Logfile_Type::ERRORLOG, __FUNCTION__, "----------------------------RoundZeroLogger");

	yLog::logtime(Logfile_Type::DEBUGLOG, __FUNCTION__, "----------------------------RoundZeroLogger");
		
	yLog::log(WLREPORTLOG, " JobID \t       size \t    Arrival \t  Departure \t     Energy \t ExecutionT \t  ResponseT \t   Slowdown \t AvgSpeed");
	
	//yLog::log(BYTESLOG, "%10f \t %10f", ...);
	yLog::log(BYTELOG, " Timestamp  \t Bytes In System");
	yLog::log(SPEEDLOG, " Timestamp  \t Execution Speed");
	yLog::log(JOBCOUNTLOG, " Timestamp  \t Job Count in System");
}

//----------------------------------------------------------------------

// Clean up and say done.
RoundZeroLogger::~RoundZeroLogger() {
	clear(WLREPORTLOG);
	yLog::log(SPEEDLOG, "%10f \t 0", prevTime_m);
	yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "----------------------------RoundZeroLogger done!");

	yLog::logtime(Logfile_Type::ERRORLOG, __FUNCTION__, "----------------------------RoundZeroLogger done!");

	yLog::logtime(Logfile_Type::DEBUGLOG, __FUNCTION__, "----------------------------RoundZeroLogger done!");
	yLog::flushall();
}

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string RoundZeroLogger::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event
void RoundZeroLogger::arrival_handler(ArrivalEvent * e) {
	jobCount_m++;
	// yLog::log(JOBCOUNTLOG, " Timestamp  \t Job Count in System");
	yLog::log(JOBCOUNTLOG, "%10f \t %10d", e->time, jobCount_m);

	byteCount_m += e->job_m.getSize();
	//yLog::log(BYTELOG, " Timestamp  \t Bytes In System");
	yLog::log(BYTELOG, "%10f \t %10f", e->time, byteCount_m);
}

//----------------------------------------------------------------------

// Handles a departure event, must free the job
void RoundZeroLogger::departure_handler(DepartureEvent * e, Job * job) {
	avgE_m += job->energyConsumed_m;
	avgT_m += job->departure_m - job->getArrival();
	totalN_m ++;
	firstNjobsLeft_m++;

	if (inPromptMode_m) 
		writeJobReport(job, WLREPORTLOG, job->getProbeID());
	else
		completedJobs_m.push_back(job);

	jobCount_m--;
	// yLog::log(JOBCOUNTLOG, " Timestamp  \t Job Count in System");
	yLog::log(JOBCOUNTLOG, "%10f \t %10d", e->time, jobCount_m);
}

//----------------------------------------------------------------------

// Handles a speed-change event
void RoundZeroLogger::speedchange_handler(double time, double newspeed) {
	if (prevSpeed_m != newspeed) {
		yLog::log(SPEEDLOG, "%10f \t %10f", prevTime_m, newspeed);
	}

	// ? Debug required. Changed after PEVA submission. The following two assigments where taken out of the if.
	prevTime_m = time;
	prevSpeed_m = newspeed;
}

//----------------------------------------------------------------------

// Handles recording of the executions 
void RoundZeroLogger::execution_handler(std::vector<unsigned long> & jobs, std::vector<double> & ratePerJob, double time1, double time2) {
	if (!approximatelyEqual(prevExecutionTime_m, time1) && !zeroRemaining_m)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Missing gap [%10f, %10f] from last update.", prevExecutionTime_m, time1);
	double rate = 0;
	for (unsigned i = 0; i < ratePerJob.size(); i++)
		rate += ratePerJob[i];
	
	prevExecutionTime_m = time2;
	byteCount_m -= (time2 - time1) * rate;
	if (approximatelyEqual(byteCount_m, 0)) {
		byteCount_m = 0;
		zeroRemaining_m = true;
	}
	else if (definitelyLessThan(byteCount_m, 0)) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Negative byteCount value! %f", byteCount_m);
		byteCount_m = 0;
		zeroRemaining_m = true;
	}
	else
			zeroRemaining_m = false;
	//yLog::log(BYTELOG, " Timestamp  \t Bytes In System");
	yLog::log(BYTELOG, "%10f \t %10f", time2, byteCount_m);
}
		

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------



