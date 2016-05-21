
#ifndef _FSP_h_

	#define _FSP_h_

/*----------------------------------------------------------------------
* FSP.h - definitions/declarations for class FSP
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file defines the class FSP, with its members and methods.
*
*///---------------------------------------------------------------------

	#include <queue>
	#include <string>

	#include "PS.h"
	#include "Event.h"
	#include "DESLogger.h"
	#include "Scheduler.h"
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class FSP: public Scheduler {
	public:
		FSP();
		~FSP();

	public:
		std::string toString();		// Returns the name and parameters of the object

		bool arrival_handler(ArrivalEvent *);			// Handles an arrival event
		Job * departure_handler(DepartureEvent *);		// Handles a departure event
		bool speedchange_handler(SpeedChangeEvent *);	// Handles a speed-change event
		Event * bonusevent_handler(SchedulerEvent *, double speed);		// Handles a bonus event, return true if handled
		
		unsigned long getExJob(double time);					// Returns the job-ID of the smallest job under execution
		Event * nextDeparture(double speed, double time);		// Creates a new departure Event object and retunrs its pointer
		Event * nextScheduler(double speed, double time);		// Creates a new scheduler event, place holder for unknown desings

		void updatePeriod(double time1, double time2, double speed, DESLogger *, PowerFunction *);	/* Applies the passage of time.
																										- Parameter one (time1) should match the 
																										internal value of previous update time. 
																										- Parameter three (speed), gives the speed 
																										during interval [time1, time2]
																										- DESLogger * is used for simulation logs 
																										- Must call DESLogger.execution_handler*/

	public:
		class CompareJobFSP {
		public:
			//// Returns true if j1 should run earlier than j2 (if set is used)
			//bool operator()(const Job * j1, const Job * j2) const {		
			//	if (j1->isBeingExecuted_m && j2->isBeingExecuted_m) 
			//		yLog::logtime(ERRORLOG, __FUNCTION__, "Two jobs are being executed at the same time!");
			//	if (j1->isBeingExecuted_m || j2->isBeingExecuted_m)
			//		return j1->isBeingExecuted_m;
			//	else if (approximatelyEqual(j1->remsize_m, j2->remsize_m))
			//		return definitelyLessThan(j1->getArrival(), j2->getArrival());
			//	else
			//		return definitelyLessThan(j1->remsize_m, j2->remsize_m); 
			//}
			// Returns false if j1 should run earlier than j2 (if priority queue is used)
			bool operator()(const Job * j1, const Job * j2) const {	
				// The job under execution should not be in the jobs queue!
				if (j1->isBeingExecuted_m || j2->isBeingExecuted_m) 
					yLog::logtime(ERRORLOG, __FUNCTION__, "There are jobs marked as being executed in the jobs queue!");

				if (approximatelyEqual(j1->remsize_m, j2->remsize_m)) {
					if (approximatelyEqual(j1->getArrival(), j2->getArrival()))
						return j1->getID() > j2->getID();
					else
						return definitelyGreaterThan(j1->getArrival(), j2->getArrival());
				}
				else
					return definitelyGreaterThan(j1->remsize_m, j2->remsize_m); 
			}
		};

	private:
		//std::set<Job *, CompareJobFSP> jobs_q; // the sorted queue, sorted based on job sizes
		std::priority_queue<Job*, std::vector<Job*>, CompareJobFSP> jobs_q;		// All jobs in the system excluding the exJob_m
		Job * exJob_m;		// The job under execution
		PS * vPS_m;			// The virtual PS

	private:
		static string name_s;
	};

//----------------------------------------------------------------------

#endif