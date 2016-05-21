/***
* ConfigurationReader.cpp - implementation of ConfigurationReader class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the ConfigurationReader class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "yLog.h"
#include "Event.h"
#include "GlobalsAndTypes.h"
#include "DESLogger.h"
#include "Configuration.h"
#include "BasicLogger.h"
#include "RoundZeroLogger.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;


//----------------------------------------------------------------------

Configuration::Configuration() {
	// Anything to do?
}

//----------------------------------------------------------------------

Configuration::~Configuration() {
	// Anything to do?
}

//----------------------------------------------------------------------

DESLogger * Configuration::configurationReader(string conf_file) {
	ifstream config (conf_file.c_str());
	if (config.fail()) {
		cerr << "**Could not open the configuration file. Exiting the Simualation." << endl;
		return NULL;
	}

	string line;
	getline(config, line);
	if (line.length() == 0) {
		cerr << "**Empty configuration file. Exiting the Simualation." << endl;
		return NULL;
	}

	cout << "Reading the config file:" << endl;

	while (line.length() != 0) {
		if (line[0] == '%') {
			getline(config, line);
			continue;
		}
		string key;
		stringstream tokenizer;
		tokenizer << line;
		tokenizer >> key;
		if (key == "PATH") {
			tokenizer >> PATH_m;
			cout << line << endl;
		}
		else if (key == "SCHEDULER") {
			 tokenizer >> simName_m;
			 SCHEDULER_m = line;
			 cout << line << endl;
		}
		else if (key == "SPEEDSCALER") {
			 string type;
			 tokenizer >> type >> type;
			 simName_m+= string("-") + type + string("-");
			 SPEEDSCALER_m = line;
			 cout << line << endl;
		}
		else if (key == "POWERFUNCTION") {
			 POWERFUNCTION_m = line;
			 cout << line << endl;
		}
		else if (key == "WORKLOAD") {
			 string type, maxN;
			  tokenizer >> type >> maxN;
			 simName_m+= string("-") + type + string("-") + maxN + string("-");
			 WORKLOAD_m = line;
			 cout << line << endl;
		}
		else if (key == "LOGGER") {
			 LOGGER_m = line;
			 cout << line << endl;
		}
		else if (key == "PROBES") {
			 PROBES_m = line;
			 cout << line << endl;
		}
		else {
			 cerr << "**Invalid Configuration file format. Exiting the Simualation." << endl;
		}
		getline(config, line);
	}
	return DESLoggerFactory(getLoggerConfig());
}

//----------------------------------------------------------------------

string Configuration::printSimParam() {
	stringstream ss;
	ss	<< "PATH " << PATH_m << endl
		<< SCHEDULER_m << endl
		<< SPEEDSCALER_m << endl
		<< POWERFUNCTION_m << endl
		<< WORKLOAD_m << endl
		<< LOGGER_m << endl
		<< PROBES_m << endl;
	return ss.str();
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
	// Construct an Scheduler object based on the input parameters
	// Returns NULL object on fail
Scheduler * Configuration::SchedulerFactory(std::string s) {
	stringstream ss;
	ss << s;
	string type;
	ss >> type >> type;

	Scheduler * obj = NULL;

	if (type == "SRPT") {
		obj = new SRPT();
		return obj;
	}
	
	if (type == "PS") {
		obj = new PS();
		return obj;
	}

	if (type == "FSP") {
		obj = new FSP();
		return obj;
	}

	if (type == "LRPT") {
		obj = new LRPT();
		return obj;
	}
	
	if (type == "FCFS") {
		obj = new FCFS();
		return obj;
	}

	if (type == "SJF") {
		obj = new SJF();
		return obj;
	}

	//if (type == "") {
	//	return obj;
	//}

	return obj;
}

//----------------------------------------------------------------------

// Construct an SpeedScaler object based on the input parameters
// Returns NULL object on fail
SpeedScaler * Configuration::SpeedScalerFactory(std::string s, std::string powerStr) {
	stringstream ss;
	ss << s;
	string type;
	ss >> type >> type;

	SpeedScaler * obj = NULL;

	if (type == "SingleSpeed") {
		double speed, basespeed;
		ss >> speed >> basespeed;
		obj = new SingleSpeed(speed, basespeed, PowerFunctionFactory(powerStr));
		return obj;
	}
	
	if (type == "CoupledSpeed") {
		string functionType;
		double basespeed;
		ss >> functionType >> basespeed;
		obj = new CoupledSpeed(basespeed, PowerFunctionFactory(powerStr));
		return obj;
	}

	if (type == "ShadowSpeed") {
		string shadowShedulerType;
		double basespeed;
		ss >> shadowShedulerType >> basespeed;
		obj = new ShadowSpeed(basespeed, PowerFunctionFactory(powerStr), SchedulerFactory(shadowShedulerType));
		return obj;
	}

	if (type == "LRPTSpeed") {
		string functionType;
		double basespeed;
		ss >> functionType >> basespeed;
		obj = new LRPTSpeed(basespeed, PowerFunctionFactory(powerStr));
		return obj;
	}

	if (type == "RandomSpeed") {
		string functionType;
		double basespeed;
		ss >> functionType >> basespeed;
		obj = new RandomSpeed(basespeed, PowerFunctionFactory(powerStr));
		return obj;
	}

	if (type == "ExternalSpeed") {
		return obj;
	}

	//if (type == "") {
	//	return obj;
	//}

	return obj;
}

//----------------------------------------------------------------------

// Construct an WorkloadGenerator object based on the input parameters
// Returns NULL object on fail
WorkloadGenerator * Configuration::WorkloadGeneratorFactory(std::string s) {
	stringstream ss;
	ss << s;
	string type;
	ss >> type >> type;

	WorkloadGenerator * obj = NULL;

	if (type == "FILE") {
		unsigned long maxN;
		ss >> maxN >> s;
		obj = new WorkloadFileWrapper(s, maxN);
		return obj;
	}
	
	if (type == "EXPONENTIAL") {
		unsigned long maxN;
		double arrival, size;
		ss >> maxN >> arrival >> size;
		obj = new WLGenExponential(arrival, size, maxN);
		return obj;
	}
	if (type == "PARETO") {
		unsigned long maxN;
		double arrival, alpha;
		ss >> maxN >> arrival >> alpha;
		obj = new WLGenPareto(arrival, alpha, maxN);
		return obj;
	}
	if (type == "CUSTOM") {
		unsigned long maxN;
		double arrival, size;
		ss >> maxN >> arrival >> size;
		obj = new WLGenCustom(arrival, size, maxN);
		return obj;
	}

	if (type == "RANDOMBATCH") {
		unsigned long maxN;
		double batchCountMean, batchIntervalMean, size;
		ss >> maxN >> batchCountMean >> batchIntervalMean >> size;
		obj = new WLGenBatchExponential(batchCountMean, batchIntervalMean, size, maxN);
		return obj;
	}
	//if (type == "") {
	//	return obj;
	//}

	return obj;
}

//----------------------------------------------------------------------

// Construct an Scheduler object based on the input parameters
// Returns NULL object on fail
DESLogger * Configuration::DESLoggerFactory(std::string s) {
	stringstream ss;
	ss << s;
	string type;
	ss >> type >> type;

	DESLogger * obj = NULL;

	if (type == "BasicLogger") {
		string typeparam, timeparam;
		ss >> typeparam >> timeparam;
		obj = new BasicLogger(typeparam, timeparam, PATH_m, simName_m);
		return obj;
	}
	

	if (type == "RoundZeroLogger") {
		string typeparam, timeparam;
		ss >> typeparam >> timeparam;
		obj = new RoundZeroLogger(typeparam, timeparam);
		return obj;
	}
	//if (type == "") {
	//	return obj;
	//}

	return obj;
}

//----------------------------------------------------------------------

// Construct an ProbeSetting object based on the input parameters
// Returns NULL object on fail
//ProbeSetting * Configuration::ProbeSettingFactory(std::string s) {
//	stringstream ss;
//	ss << s;
//	string type;
//	ss >> type >> type;
//	
//	ProbeSetting * obj = NULL;
//	
//	if (type == "PROBES") {
//		unsigned int rounds;
//		double step, smallest, largest;
//		obj = new ProbeSetting(rounds, step, smallest, largest);
//		return obj;
//	}
//	return obj;
//}

//----------------------------------------------------------------------

// Construct an PowerFunction object based on the input parameters
// Returns NULL object on fail
PowerFunction * Configuration::PowerFunctionFactory(std::string s) {
	stringstream ss;
	ss << s;
	string type;
	ss >> type >> type;
	
	PowerFunction * obj = NULL;
	
	if (type == "ALPHA") {
		double alpha;
		ss >> alpha;
		obj = new AlphaPowerFunction(alpha);
		return obj;
	}
	//if (type == "") {
	//	return obj;
	//}
	return obj;
}

//----------------------------------------------------------------------



		
