
#ifndef _SERPT_h_

	#define _SERPT_h_
/*----------------------------------------------------------------------
* SERPT.h - definitions/declarations for class SERPT
*
*       Written by Mithun P
*		Last Updated: July 2022
*
*Purpose:
*       This file defines the class SERPT, with its members and methods.
*
*///---------------------------------------------------------------------

#include <vector>
#include <queue>
#include <string>

#include "Scheduler.h"
#include "Job.h"

//----------------------------------------------------------------------


class SERPT : public Scheduler {
public:
	SERPT();
	~SERPT();

public:
	std::string toString();		// Returns the name and parameters of the object

	bool arrival_handler(ArrivalEvent*);				// Handles an arrival event
	Job* departure_handler(DepartureEvent*);			// Handles a departure event, returns the summary of Job
	bool speedchange_handler(SpeedChangeEvent*);		// Handles a speed-change event
	Event* bonusevent_handler(SchedulerEvent*, double speed);			// Handles a bonus event, return true if handled

	unsigned long getExJob(double time);						// Returns the job-ID of the current job under execution
	Event* nextDeparture(double speed, double time);			// Creates a new departure Event object and retunrs its pointer
	Event* nextScheduler(double speed, double time);		// Creates a new scheduler event, place holder for unknown desings

	// Already implemented in the base class Scheduler
	//bool isOverwritten(DepartureEvent *);	// Returns true if the event is overwritten
	//bool isOverwritten(SchedulerEvent *);	// Returns true if the event is overwritten

	void updatePeriod(double time1, double time2, double speed, DESLogger*, PowerFunction* P);	/* Applies the passage of time.
																									- Parameter one (time1) should match the
																									internal value of previous update time.
																									- Parameter three (speed), gives the speed
																									during interval [time1, time2]
																									- DESLogger * is used for simulation logs */
public:
	class CompareJobSERPT {
	public:
		// Returns false if j1 should run earlier than j2 
		bool operator()(const Job* j1, const Job* j2) const {
			if (approximatelyEqual(j1->estimatedremsize_m, j2->estimatedremsize_m)) {
				if (j1->isBeingExecuted_m)
					return false;
				else if (j2->isBeingExecuted_m)
					return true;
				else
					return definitelyGreaterThan(j1->getArrival(), j2->getArrival());
			}
			else
				return definitelyGreaterThan(j1->estimatedremsize_m, j2->estimatedremsize_m);
		}
	};

private:
	std::priority_queue<Job*, std::vector<Job*>, CompareJobSERPT> jobs_q;	// The queue of jobs in the system
	//std::priority_queue<Job*, std::vector<Job*>, CompareJobSERPT> jobs_q;	// The queue of jobs in the system

private:
	static string name_s;
};

//----------------------------------------------------------------------
//----------------------------------------------------------------------

#endif