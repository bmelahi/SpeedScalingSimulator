/***
* ProbeSetting.cpp - implementation of ProbeSetting class
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of the ProbeSetting class.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )


#include <sstream>
#include <iostream>

#include "DES.h"
#include "Configuration.h"
#include "ProbeSetting.h"
#include "Event.h"
#include "yLog.h"
#include "PRandomGen.h"

using namespace std;
	
//----------------------------------------------------------------------

ProbeSetting::ProbeSetting(DESLogger * logger, Configuration * config) 
	: logger_m(logger), config_m(config) {
	stringstream ss;
	ss << config_m->getProbesConfig();
	string type;
	ss >> type;
	if (type == "PROBES") 
		ss >> rounds_m >> stepsize_m >> smallest_m >> largest_m;
	else
		yLog::logtime(ERRORLOG, __FUNCTION__, "Unable to initialize the ProbeSetting object from the config file.");
		
	//+ Anything else to do?
}

//----------------------------------------------------------------------

ProbeSetting::~ProbeSetting() {
	//+ Anything else to do?
}

//----------------------------------------------------------------------

bool ProbeSetting::run() {

	// Round zero, simulate the workload without the probe.
	// First construct to config string to pass to the object factory
	stringstream ssConfig, ssModifed;
	string type, configType, param1, param2;
	ssConfig << config_m->getLoggerConfig();
	ssConfig >> type >> configType >> param1 >> param2;
	ssModifed << type << " RoundZeroLogger " << param1 << " " << param2;
	DESLogger * rzLogger = config_m->DESLoggerFactory(ssModifed.str());

	DES roundZero(
		config_m->SchedulerFactory(config_m->getSchedulerConfig()),
		config_m->SpeedScalerFactory(config_m->getSpeedscalerConfig(), config_m->getPowerfunctionConfig()),
		config_m->WorkloadGeneratorFactory(config_m->getWorkloadConfig()),
		rzLogger,
		config_m->PowerFunctionFactory(config_m->getPowerfunctionConfig())
		);

	double workload_sim_time = roundZero.run();
	yLog::flushall();
	delete rzLogger;

	double maxProbeSimLength = 0;
	double probeSize = smallest_m;
	unsigned long probeID = 0;
	
	vector<Job *> completedProbes_m;

	while (probeSize <= largest_m) {
		cout << "Probe size: " << probeSize << endl;
		for (unsigned i = 1; i <= rounds_m; i++) {
			//cout << "Probe size: " << probeSize << " - round: " << i << endl;
			//ssModifed.clear();
			//ssModifed << type << "probeRoundLogger" << param1 << param2;

			double probeInsertTime = computeRandomProbeInsertionTime(workload_sim_time, rounds_m, i);
			probeID++;
			DES probeRound (
				config_m->SchedulerFactory(config_m->getSchedulerConfig()),
				config_m->SpeedScalerFactory(config_m->getSpeedscalerConfig(), config_m->getPowerfunctionConfig()),
				config_m->WorkloadGeneratorFactory(config_m->getWorkloadConfig()),
				//config_m->DESLoggerFactory(config_m->getLoggerConfig()),
				logger_m,
				config_m->PowerFunctionFactory(config_m->getPowerfunctionConfig()),
				probeSize, probeInsertTime, probeID
				);
			double len = probeRound.run();
			if (len > maxProbeSimLength)
				maxProbeSimLength = len;

			//completedProbes_m.push_back(probeRound.getCompletedProbe());
		}
		probeSize += stepsize_m;
	}

	yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "Closing DES: Discrete Event Simulator. Wrapping up...");

	return true;
}

//----------------------------------------------------------------------

bool ProbeSetting::runLoad() {

	// Round zero, simulate the workload without the probe.
	// First construct to config string to pass to the object factory
	stringstream ssConfig, ssModifed;
	string type, configType, param1, param2;
	ssConfig << config_m->getLoggerConfig();
	ssConfig >> type >> configType >> param1 >> param2;
	ssModifed << type << " RoundZeroLogger " << param1 << " " << param2;
	
	for (double i = 0.25; i <= 3; i+= 0.25) {
		stringstream wlConfig, wlModifed;
		string type, configType, maxN, lambda, mu;
		wlConfig << config_m->getWorkloadConfig();
		wlConfig >> type >> configType >> maxN >> lambda >> mu;
		wlModifed << type << " " << configType << " " << maxN <<  " " << i << " " << mu;
		//wlModifed << type << " " << configType << " " << maxN <<  " " << (double)pow(2.0, (double)i)  << " " << mu;
		//DESLogger * rzLogger = config_m->DESLoggerFactory(ssModifed.str());
		DESLogger * rzLogger = config_m->DESLoggerFactory(ssModifed.str());
	
		DES roundZero(
			config_m->SchedulerFactory(config_m->getSchedulerConfig()),
			config_m->SpeedScalerFactory(config_m->getSpeedscalerConfig(), config_m->getPowerfunctionConfig()),
			config_m->WorkloadGeneratorFactory(wlModifed.str()),
			rzLogger,
			config_m->PowerFunctionFactory(config_m->getPowerfunctionConfig())
			);

		double workload_sim_time = roundZero.run();
		delete rzLogger;
	}
	


	yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "Closing DES: Discrete Event Simulator. Wrapping up...");

	return true;
}

//----------------------------------------------------------------------

double ProbeSetting::computeRandomProbeInsertionTime(double workload_sim_time, unsigned int rounds, unsigned thisround) {
	double startPeriod = workload_sim_time / 3.0;
	double endPeriod = workload_sim_time * 2.0 / 3.0;

	//return PRandomGen::getUrandFrac() * (endPeriod - startPeriod) + startPeriod;
	double insert = startPeriod + (endPeriod - startPeriod) / rounds * thisround;
	return insert;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

