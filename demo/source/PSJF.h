
#ifndef _PSJF_h_

	#define _PSJF_h_

/*----------------------------------------------------------------------
* PSJF.h - definitions/declarations for class PSJF
*
*       Tutorial example: adding a new scheduling policy to S3.
*
* Purpose:
*       Preemptive Shortest Job First. Jobs are served in increasing
*       order of their ORIGINAL size, and the decision is re-made at
*       every arrival - so a newly arrived job that is smaller than the
*       one in service preempts it.
*
*       This sits exactly between two policies the simulator already has:
*
*         SJF   - orders by original size,  non-preemptive
*         PSJF  - orders by original size,  preemptive        <-- this file
*         SRPT  - orders by remaining size, preemptive
*
*       Comparing the three isolates the two independent design choices:
*       what you prioritise on (original vs remaining size), and when you
*       are allowed to switch (only at completion vs at every arrival).
*
*///---------------------------------------------------------------------

	#include <vector>
	#include <queue>
	#include <string>

	#include "Scheduler.h"
	#include "Job.h"

//----------------------------------------------------------------------

	class PSJF: public Scheduler {
	public:
		PSJF();
		~PSJF();

	public:
		std::string toString();		// Returns the name and parameters of the object

		bool arrival_handler(ArrivalEvent *);				// Handles an arrival event
		Job * departure_handler(DepartureEvent *);			// Handles a departure event, returns the summary of Job
		bool speedchange_handler(SpeedChangeEvent *);		// Handles a speed-change event
		Event * bonusevent_handler(SchedulerEvent *, double speed);		// Handles a bonus event, return true if handled

		unsigned long getExJob(double time);					// Returns the job-ID of the current job under execution
		Event * nextDeparture(double speed, double time);		// Creates a new departure Event object and returns its pointer
		Event * nextScheduler(double speed, double time);		// Creates a new scheduler event, place holder for unknown designs

		void updatePeriod(double time1, double time2, double speed, DESLogger *, PowerFunction * P);	/* Applies the passage of time.
																										- Parameter one (time1) should match the
																										internal value of previous update time.
																										- Parameter three (speed), gives the speed
																										during interval [time1, time2]
																										- DESLogger * is used for simulation logs */
	public:
	class CompareJobPSJF {
	public:
		// Returns false if j1 should run earlier than j2.
		// Ordering is by ORIGINAL size (getSize()), which is what makes this
		// PSJF rather than SRPT - SRPT uses remsize_m here instead. Ties are
		// broken in favour of the job already running (avoids pointless
		// context switches between equal-size jobs), then by earlier arrival.
		bool operator()(const Job * j1, const Job * j2) const {
			if (approximatelyEqual(j1->getSize(), j2->getSize())) {
				if (j1->isBeingExecuted_m)
					return false;
				else if (j2->isBeingExecuted_m)
					return true;
				else
					return definitelyGreaterThan(j1->getArrival(), j2->getArrival());
			}
			else
				return definitelyGreaterThan(j1->getSize(), j2->getSize());
		}
	};

	private:
		std::priority_queue<Job*, std::vector<Job*>, CompareJobPSJF> jobs_q;	// The queue of jobs in the system

	private:
		static string name_s;
	};

//----------------------------------------------------------------------
//----------------------------------------------------------------------

#endif
