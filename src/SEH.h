#ifndef _SEH_h_

#define _SEH_h_

/*----------------------------------------------------------------------
* SEH.h - definitions/declarations for class SEH
*       Written by Mithun P
*		Last (remembered) updated: July 2022
*
*Purpose:
*       This file defines the class SEH, with its members and methods.
*
*///---------------------------------------------------------------------
#include <vector>
#include <queue>
#include <string>

#include "Scheduler.h"
#include "Job.h"

//----------------------------------------------------------------------


class SEH : public Scheduler {
public:
	SEH();
	~SEH();

public:
	std::string toString();		// Returns the name and parameters of the object

	bool arrival_handler(ArrivalEvent*);				// Handles an arrival event
	Job* departure_handler(DepartureEvent*);			// Handles a departure event, returns the summary of Job
	bool speedchange_handler(SpeedChangeEvent*);		// Handles a speed-change event
	Event* bonusevent_handler(SchedulerEvent*, double speed);			// Handles a bonus event, return true if handled

	unsigned long getExJob(double time);						// Returns the job-ID of the current job under execution
	Event* nextDeparture(double speed, double time);			// Creates a new departure Event object and retunrs its pointer
	Event* nextScheduler(double speed, double time);		// Creates a new scheduler event, place holder for unknown desings


	void updatePeriod(double time1, double time2, double speed, DESLogger*, PowerFunction* P);	/* Applies the passage of time.
																									- Parameter one (time1) should match the
																									internal value of previous update time.
																									- Parameter three (speed), gives the speed
																									during interval [time1, time2]
																									- DESLogger * is used for simulation logs */
public:
	class CompareJobSEH {
	public:
		// Compare SEH index scores of j1 and j2 using which function returns false if j1 should run earlier than J2
		bool operator()(const Job* j1, const Job* j2) const {
			if (approximatelyEqual(j1->sehIndexScore_m, j2->sehIndexScore_m)) {
				if (j1->isBeingExecuted_m)
					return false;
				else if (j2->isBeingExecuted_m)
					return true;
				else
					return definitelyGreaterThan(j1->getArrival(), j2->getArrival());
			}
			else
				return definitelyLessThan(j1->sehIndexScore_m, j2->sehIndexScore_m);
			// Higher index score takes higher priority, 
			// hence if j1 has a lesser index score than j2 
			// then there will a change in execution
		}

	};
private:
	std::priority_queue<Job*, std::vector<Job*>, CompareJobSEH> jobs_q;	// The queue of jobs in the system

private:
	static string name_s;

};
//----------------------------------------------------------------------

#endif