
#ifndef _ProbeSetting_h_

	#define _ProbeSetting_h_

/*----------------------------------------------------------------------
* ProbeSetting.h - definitions/declarations for class ProbeSetting
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file defines the class ProbeSetting, with its members and methods.
*		
*		
*///--------------------------------------------------------------------

	#include "Job.h"
	#include "Event.h"
	#include "DESLogger.h"
	#include "Scheduler.h"
	#include "SpeedScaler.h"
	#include "Configuration.h"
	#include "WorkLoadGenerator.h"
	
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class ProbeSetting {
	public:
		//DES (SCHEDULER_TYPE, SPEEDSCALAR_TYPE, WORKLOADGENERATOR_TYPE, LOG_GENERATOR);
		ProbeSetting (DESLogger * logger, Configuration * config);
		~ProbeSetting();

	public:
		bool run();
		bool runLoad();

	private:
		unsigned int getRounds() {return rounds_m;} 
		double getStepSize() {return stepsize_m;}
		double getSmallest() {return smallest_m;}
		double getLargest() {return largest_m;}

		double computeRandomProbeInsertionTime(double workload_sim_time, unsigned int round, unsigned int thisround);
		
	private:
		unsigned int rounds_m;
		double stepsize_m;
		double smallest_m;	
		double largest_m;

		DESLogger * logger_m;
		Configuration * config_m;
	};

//----------------------------------------------------------------------
#endif