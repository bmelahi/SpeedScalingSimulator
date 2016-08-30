
#ifndef _PS_h_

	#define _PS_h_

/*----------------------------------------------------------------------
* PS.h - definitions/declarations for class PS
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file defines the class PS, with its members and methods.
*
*///---------------------------------------------------------------------

	#include <set>
	#include <string>

	#include "Event.h"
	#include "DESLogger.h"
	#include "Scheduler.h"
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class PS: public Scheduler {
	public:
		PS();
		~PS();

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
		class CompareJobPS {
		public:
			// Returns true if j1 should run earlier than j2 
			bool operator()(const Job * j1, const Job * j2) const {		
				if (approximatelyEqual(j1->remsize_m, j2->remsize_m)) {
					if (approximatelyEqual(j1->getArrival(), j2->getArrival()))
						return j1->getID() < j2->getID();
					else
						return definitelyLessThan(j1->getArrival(), j2->getArrival());
				}
				else
					return definitelyLessThan(j1->remsize_m, j2->remsize_m); 
			}
		};

		bool setFinish(unsigned long id, double times);		// Set the isFinished of job with parameter id. Returns true if the job is found.

	protected:
		std::set<Job *, CompareJobPS> jobs_q; // the sorted queue, sorted based on job sizes

	private:
		static string name_s;

	//protected:
	//	double lastUpdate_m;					// Keeps the time of the end of last updatePeriod
	//	unsigned long validDepartureEvent_m;	// Kept for departure event overwrite checks	
	//	unsigned long validSchedulerEvent_m;	// Kept for scheduler event overwrite checks	

	//public:
	//	unsigned long nextValidDepartureID()			// Returns the next valid departure id
	//	{return ++validDepartureEvent_m;}
	//	unsigned long nextValidSchedulerID()			// Returns the next valid departure id
	//	{return ++validSchedulerEvent_m;}
	};

//----------------------------------------------------------------------

#endif