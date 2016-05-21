
#ifndef _DES_h_

	#define _DES_h_

/*----------------------------------------------------------------------
* DES.h - definitions/declarations for class DES
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
*Purpose:
*       This file defines the class DES, with its members and methods.
*		DES is a Discrete Event Simulator for a service queue
*		It is initialized with three objects of the following classes:
*			- Scheduler
*			- SpeedScaler
*			- WorkloadGenerator
*		and produces a log of events and their detail.
*
*		Event log format (one line per event): "<time of event> <event type> <event details>"
*			- <time of event>: is relative to the start of simulation at time zero
*			- <event type>: as defined in the GlobalsAndTypes.h
*			- <event details>: depends on the event type, should show the job that has caused the event, or other cause of event
*		
*		
*///--------------------------------------------------------------------

	#include <map>
	#include <list>
	#include <queue>

	#include "Job.h"
	#include "Event.h"
	#include "DESLogger.h"
	#include "Scheduler.h"
	#include "SpeedScaler.h"
	#include "WorkLoadGenerator.h"
	
	#include "GlobalsAndTypes.h"

	class DES {
	public:
		//DES (SCHEDULER_TYPE, SPEEDSCALAR_TYPE, WORKLOADGENERATOR_TYPE, LOG_GENERATOR);
		DES (Scheduler *, SpeedScaler *, WorkloadGenerator *, DESLogger *, PowerFunction *);
		DES (Scheduler *, SpeedScaler *, WorkloadGenerator *, DESLogger *, PowerFunction *,
			double probeSize, double probeInsertTime, unsigned long probeID);
		~DES();

	public:
		double run();		// Returns the time of the last event
		Job * getCompletedProbe();	// Returns the Job object with the report of probe job's execution.


	private:				// Handlers for different events
		void arrival_handler(Event *);
		void departure_handler(Event *);
		void scheduler_handler(Event *);
		void speedchange_handler(Event *);

	private:
		Scheduler * scheduler_m;
		SpeedScaler * speedscaler_m;
		WorkloadGenerator * workload_m;
		DESLogger *	logger_m;
		PowerFunction * P_m;

		//map <unsigned long int, Job> jobs_m;		// Probably not needed.
		priority_queue<Event, std::vector<Event*>, Event::CompareEvents> events_q;

		double exRate_m;			// Execution rate: since the previous context switch/speed change
		unsigned long exJob_m;		// Executing job id: since the previous context switch/speed change
		double exRemaining_m;		// Executing job: remaining work at the previous context switch/speed change
		double exEventTime;			// Previous context switch/speed change
		unsigned long probeID_m;	// The internal probeID = 1


	private:
		void statusUpdate(double time);
		bool isProbe(unsigned long id);
	};

//----------------------------------------------------------------------
#endif