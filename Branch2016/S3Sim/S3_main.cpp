/***
* S3_main.cpp - The Solutions main file
*
*       Written by Maryam Elahi
*		Last updated: Oct 30, 2011
*
* Purpose:
*       This is the main file of the solution.
*
****/

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include <Windows.h>

#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <queue>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "GlobalsAndTypes.h"
#include "DES.h"
#include "yLog.h"
#include "SRPT.h"
#include "Job.h"

using namespace std;

//----------------------------------------------------------------------

const string conf_file = "S3Config.dat";

//----------------------------------------------------------------------

void setSimulationFolderNameByTime(wchar_t * outputFolder);

//----------------------------------------------------------------------

int main (int argc, int* argv) {

	// Start of a new simulation: say hi!
	cout << "=========================================================" << endl
		 << "Welcom to S3: The Speed Scaling Simulation!" << endl
		 << "=========================================================" << endl
		 << endl;

	// Set simulation folder name
	wchar_t * outputFolder = new wchar_t[100];
	setSimulationFolderNameByTime(outputFolder);			
	wprintf(L"Creating the simulation results folder at: \"%ls\"\n", outputFolder);

	// Create simulation folder and set default path
	if (CreateDirectory((LPCWSTR)outputFolder, NULL)) {
		_wchdir(outputFolder);
	} else {
		 cout << "Failed to create the simulation results folder!" << endl;
		 return 0;
	}

	// Setup the main log file
	yLog::setLogFile("DESLog.txt", 0);
	yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "Setting up DES: Discrete Event Simulator. This is the progress log.");

	yLog::setLogFile("errorLog.txt", 1);
	yLog::logtime(Logfile_Type::ERRORLOG, __FUNCTION__, "Setting up DES: Discrete Event Simulator. This is the error log.");

	yLog::setLogFile("debugLog.txt", 2);
	yLog::logtime(Logfile_Type::DEBUGLOG, __FUNCTION__, "Setting up DES: Discrete Event Simulator. This is the debug log.");

	//+ Read the configuration file
	//...
	//...
	//string scheduler_type;
	//switch (scheduler_type) {
	//	case "SRPT":
	//		Scheduler * scheduler = new SRPT();
	//		break;
	//	case "FSP":
	//		Scheduler * scheduler = new FSP();
	//		break;
	//	case "PS":
	//		Scheduler * scheduler = new PS();
	//		break;
	//	case "FCFS":
	//		Scheduler * scheduler = new FCFS();
	//		break;
	//	default:
	//		break;
	//}

	//switch (speedscaler_type) {
	//	case "singleRate":
	//		SpeedScaler * speedscaler = new SSSingleRate();
	//		break;
	//	case "coupled":
	//		SpeedScaler * speedscaler = new SSCoupled();
	//		break;
	//	case "decoupled":
	//		SpeedScaler * speedscaler = new SSDecoupledSS();
	//		break;
	//	default:
	//		break;
	//}

	//switch (WL_type) {
	//	case "Exponential":
	//		WorkloadGenerator * WL = new WLExponential();
	//		break;
	//	case "Pareto":
	//		WorkloadGenerator * WL = new WLPareto();
	//		break;
	//	case "Uniform":
	//		WorkloadGenerator * WL = new WLUniform();
	//		break;
	//	default:
	//		break;
	//}

	//Scheduler * scheduler = new SRPT();
	//SpeedScaler * speedscaler = new SSSingleRate();
	//WorkloadGenerator * WL = new WLExponential();
	//DESLogger * logger = new DESLogger();

	Job test;
	test.set_probe(10);

	return 0;
}

//----------------------------------------------------------------------

void setSimulationFolderNameByTime(wchar_t * outputFolder) {
	time_t ut;
	time (&ut);
	tm * ts = gmtime (&ut);
	wsprintf(outputFolder, L"..\\data\\%04d-%02d-%02d--%02dH%02dM%02dS", 
			1900 + ts->tm_year, ts->tm_mon + 1, ts->tm_mday,
			ts->tm_hour, ts->tm_min, ts->tm_sec);
}

