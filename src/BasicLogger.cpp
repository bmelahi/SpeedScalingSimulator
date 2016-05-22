/***
* BasicLogger.cpp - implementation of BasicLogger class (of DESLogger)
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the BasicLogger class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "yLog.h"
#include "Event.h"
#include "BasicLogger.h"
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
string BasicLogger::name_s = "BasicLogger";

//----------------------------------------------------------------------

BasicLogger::BasicLogger(string debugmode, std::string writemode, string path, string simName) {

	inDebugMode_m = (debugmode == "VERBOSE")? true : false;
	inPromptMode_m = (writemode == "PROMTWRITE")? true : false;

	// Set simulation folder name
	wchar_t * outputFolder = new wchar_t[100];
	setSimulationFolderNameByTime(path, outputFolder, simName);			
	wprintf(L"Creating the simulation results folder at: \"%ls\"\n", outputFolder);

	// Create simulation folder and set default path
	if (CreateDirectory((LPCWSTR)outputFolder, NULL)) {
		_wchdir(outputFolder);
	} else {
		 cout << "Failed to create the simulation results folder!" << endl;
		 return;
	}

	// Set the loggers based on the mode 
	yLog::setLogFile("progresslog.txt", PROGRESSLOG);
	yLog::setLogFile("probereport.txt", PROBEREPORTLOG);
	yLog::setLogFile("errorLog.txt", ERRORLOG);
	yLog::setLogFile("simparam.txt", SIMPARAMLOG);
	yLog::setLogFile("meanzlog.txt", MEANZLOG);
	yLog::setLogFile("queuedump.txt", QUEUEDUMP);
	//yLog::setLogFile("queuedump.txt", QUEUEDUMP);

	if (inDebugMode_m)
		yLog::setLogFile("debuglog.txt", DEBUGLOG);

	yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "Setting up DES: Discrete Event Simulator. This is the progress log.");
	yLog::logtime(Logfile_Type::ERRORLOG, __FUNCTION__, "Setting up DES: Discrete Event Simulator. This is the error log.");
	yLog::logtime(Logfile_Type::DEBUGLOG, __FUNCTION__, "Setting up DES: Discrete Event Simulator. This is the debug log.");
		
	yLog::log(PROBEREPORTLOG, " JobID \t       size \t    Arrival \t  Departure \t     Energy \t ExecutionT \t  ResponseT \t   Slowdown");
	yLog::log(MEANZLOG, "      E[T] \t       E[e] \t           Z \t           N");
	//yLog::log(QUEUEDUMP, "time a/d touched untouched queuedump: rem1 rem2 rem3... (arrival size)");
}

//----------------------------------------------------------------------

// Clean up and say done.
BasicLogger::~BasicLogger() {
	clear(PROBEREPORTLOG);
	yLog::logtime(PROGRESSLOG,  __FUNCTION__, "Done logging!");
}
	
//----------------------------------------------------------------------

void BasicLogger::setSimulationFolderNameByTime(string path, wchar_t * outputFolder, string simName) {
	time_t ut;
	time (&ut);
	tm * ts = gmtime (&ut);
	path = path + string("outputs\\\\") + simName;
	wsprintf(outputFolder, L"%S%04d-%02d-%02d--%02dH%02dM%02dS", path.c_str(),
			1900 + ts->tm_year, ts->tm_mon + 1, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec);
}

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string BasicLogger::toString() {
	stringstream ss;
	ss << "(" << name_s << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Handles an arrival event
void BasicLogger::arrival_handler(ArrivalEvent *) {
	jobCount_m++;
	// Nothing to do here?
}

//----------------------------------------------------------------------

// Handles a departure event, must free the job
void BasicLogger::departure_handler(DepartureEvent *, Job * job) {
	// Keep track of the avgs
	avgE_m += job->energyConsumed_m;
	avgT_m += job->departure_m - job->getArrival();
	totalN_m ++;
	firstNjobsLeft_m++;

	if (job->isProbe()) {
		if (inPromptMode_m) 
			writeJobReport(job, PROBEREPORTLOG, job->getProbeID());
		else
			completedJobs_m.push_back(job);
	}
	else
		delete job;

	jobCount_m--;
}


//----------------------------------------------------------------------

// Handles a speed-change event
void BasicLogger::speedchange_handler(double time, double newspeed) {
	// Nothing to do here?
}


//----------------------------------------------------------------------

// Handles recording of the executions 
void BasicLogger::execution_handler(std::vector<unsigned long> & jobs,	
									   std::vector<double> & ratePerJob, double time1, double time2) {
	// Nothing to do here?
}
		

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------



