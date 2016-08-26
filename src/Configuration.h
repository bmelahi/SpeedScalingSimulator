
#ifndef _Configuration_h_

	#define _Configuration_h_

/*----------------------------------------------------------------------
* WorkloadGenerator.h - definitions/declarations for class WorkloadGenerator
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file defines the class Scheduler, with its members and methods.
*
*///---------------------------------------------------------------------

	#include <string>

	#include "Event.h"
	#include "GlobalsAndTypes.h"

	#include "SpeedScaler.h"
	#include "SingleSpeed.h"
	#include "CoupledSpeed.h"
	#include "ShadowSpeed.h"
	#include "ShadowSpeedEstimateSize.h"
	#include "LRPTSpeed.h"
	#include "RandomSpeed.h"

	#include "Scheduler.h"
	#include "SRPT.h"
	#include "LRPT.h"
	#include "FCFS.h"
	#include "PSBS.h"
	#include "SJF.h"
	#include "FSP.h"
	#include "PS.h"

	#include "DESLogger.h"
	#include "BasicLogger.h"

	#include "WorkLoadGenerator.h"
	#include "WorkLoadFileWrapper.h"
	#include "WLGenExponential.h"
	#include "WLGenPareto.h"
	#include "WLGenCustom.h"
	#include "WLGenBatchExponential.h"

	#include "PowerFunction.h"
	#include "AlphaPowerFunction.h"

//----------------------------------------------------------------------

	class Configuration {
	public:
		Configuration();
		~Configuration();

	public:
		DESLogger * configurationReader(std::string conf_file);

		Scheduler * SchedulerFactory(std::string s);
		SpeedScaler * SpeedScalerFactory(std::string s, std::string powerStr);
		WorkloadGenerator * WorkloadGeneratorFactory(std::string s);
		DESLogger * DESLoggerFactory(std::string s);
		PowerFunction * PowerFunctionFactory(std::string s);

		std::string getSchedulerConfig() {return SCHEDULER_m;}
		std::string getSpeedscalerConfig() {return SPEEDSCALER_m;}
		std::string getPowerfunctionConfig() {return POWERFUNCTION_m;}
		std::string getWorkloadConfig() {return WORKLOAD_m;}
		std::string getLoggerConfig() {return LOGGER_m;}
		std::string getProbesConfig() {return PROBES_m;}
		std::string getPathConfig() {return PATH_m;}
		std::string getSimName() {return simName_m;}
		//std::string Config() {return ;}

		std::string printSimParam();

	protected:
		std::string SCHEDULER_m;
		std::string SPEEDSCALER_m;
		std::string POWERFUNCTION_m;
		std::string WORKLOAD_m;
		std::string LOGGER_m;
		std::string PROBES_m;
		std::string PATH_m;
		std::string simName_m;
	};

//----------------------------------------------------------------------

#endif