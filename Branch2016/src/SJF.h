
#ifndef _SJF_h_

	#define _SJF_h_

/*----------------------------------------------------------------------
* SJF.h - definitions/declarations for class SJF
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
* Purpose:
*       This file defines the class SJF, with its members and methods.
*
*///---------------------------------------------------------------------


#include <vector>
#include <queue>
#include <string>

#include "Scheduler.h"
#include "Job.h"

//----------------------------------------------------------------------
	

	class SJF: public Scheduler {
	public:
		SJF();
		~SJF();

	public:
		std::string toString();		// Returns the name and parameters of the object

		bool arrival_handler(ArrivalEvent *);				// Handles an arrival event
		Job * departure_handler(DepartureEvent *);			// Handles a departure event, returns the summary of Job
		bool speedchange_handler(SpeedChangeEvent *);		// Handles a speed-change event
		Event * bonusevent_handler(SchedulerEvent *, double speed);			// Handles a bonus event, return true if handled

		unsigned long getExJob(double time);						// Returns the job-ID of the current job under execution
		Event * nextDeparture(double speed, double time);			// Creates a new departure Event object and retunrs its pointer
		Event * nextScheduler(double speed, double time);		// Creates a new scheduler event, place holder for unknown desings

		// Already implemented in the base class Scheduler
		//bool isOverwritten(DepartureEvent *);	// Returns true if the event is overwritten
		//bool isOverwritten(SchedulerEvent *);	// Returns true if the event is overwritten

		void updatePeriod(double time1, double time2, double speed, DESLogger *, PowerFunction * P);	/* Applies the passage of time.
																										- Parameter one (time1) should match the
																										internal value of previous update time.
																										- Parameter three (speed), gives the speed
																										during interval [time1, time2]
																										- DESLogger * is used for simulation logs */
	public:
		class CompareJobSJF {
		public:
			// Returns false if j1 should run earlier than j2 
			bool operator()(const Job * j1, const Job * j2) const {
				if (approximatelyEqual(j1->getSize(), j2->getSize())) {
						return definitelyGreaterThan(j1->getArrival(), j2->getArrival());
				}
				else
					return definitelyGreaterThan(j1->getSize(), j2->getSize());
			}
		};

	private:
		std::priority_queue<Job*, std::vector<Job*>, CompareJobSJF> jobs_q;	// The queue of jobs in the system
		Job * current_job_m;

	private:
		static string name_s;

	};

//----------------------------------------------------------------------

#endif