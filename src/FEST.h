
#ifndef _FEST_h_

	#define _FEST_h_

/*----------------------------------------------------------------------
* FEST.h - definitions/declarations for class FEST
*
*       Written by Maryam Elahi
*		Last (remembered) updated: June 2021
*
* Purpose:
*       This file defines the class FEST, with its members and methods.
*
*///---------------------------------------------------------------------

	#include <vector>
	#include <queue>
	#include <string>

	#include "Scheduler.h"
	#include "Job.h"

//----------------------------------------------------------------------
	

	class FEST: public Scheduler {
	public:
		FEST(double load);
		~FEST();

	public:
		std::string toString();		// Returns the name and parameters of the object

		bool arrival_handler(ArrivalEvent *);				// Handles an arrival event
		Job * departure_handler(DepartureEvent *);			// Handles a departure event, returns the summary of Job
		bool speedchange_handler(SpeedChangeEvent *);		// Handles a speed-change event
		Event * bonusevent_handler(SchedulerEvent *, double speed);			// Handles a bonus event, return true if handled
		
		unsigned long getExJob(double time);						// Returns the job-ID of the current job under execution
		Event * nextDeparture(double speed, double time);			// Creates a new departure Event object and retunrs its pointer
		Event * nextScheduler(double speed, double time);		// Creates a new scheduler event, place holder for unknown desings
		
		double computeRank(const Job * j);					// Compute the rank which is the expected departure time
		void setStretch(double load);
		double getStretch() { return stretch_m; }

		void updatePeriod(double time1, double time2, double speed, DESLogger *, PowerFunction * P);	/* Applies the passage of time.
																										- Parameter one (time1) should match the 
																										internal value of previous update time. 
																										- Parameter three (speed), gives the speed 
																										during interval [time1, time2]
																										- DESLogger * is used for simulation logs */
	public:
	class CompareJobFEST {
	public:
		// Returns false if j1 should run earlier than j2 
		bool operator()(const Job * j1, const Job * j2) const {		
			if (approximatelyEqual(j1->departure_m, j2->departure_m)) {
				if (j1->isBeingExecuted_m)
					return false;
				else if (j2->isBeingExecuted_m)
					return true;
				else
					return definitelyGreaterThan(j1->getArrival(), j2->getArrival());
			}
			else
				return definitelyGreaterThan(j1->departure_m, j2->departure_m);
		}
	};

	private:
		std::priority_queue<Job*, std::vector<Job*>, CompareJobFEST> jobs_q;	// The queue of jobs in the system
		//std::priority_queue<Job*, std::vector<Job*>, CompareJobFEST> jobs_q;	// The queue of jobs in the system

	private:
		static string name_s;
		double stretch_m;
	};

//----------------------------------------------------------------------
//----------------------------------------------------------------------

#endif