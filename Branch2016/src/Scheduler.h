
#ifndef _Scheduler_h_

	#define _Scheduler_h_

/*----------------------------------------------------------------------
* Scheduler.h - definitions/declarations for class Scheduler
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
	#include "DESLogger.h"
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class Scheduler {
	public:
		Scheduler() : lastUpdate_m(0), validDepartureEvent_m(0), validSchedulerEvent_m(0) {}
		virtual ~Scheduler() = 0 {}

	public:
		virtual std::string toString() = 0;		// Returns the name and parameters of the object

		virtual bool arrival_handler(ArrivalEvent *) = 0;			// Handles an arrival event
		virtual Job * departure_handler(DepartureEvent *) = 0;		// Handles a departure event
		virtual bool speedchange_handler(SpeedChangeEvent *) = 0;	// Handles a speed-change event
		virtual Event * bonusevent_handler(SchedulerEvent *, double speed) = 0;		// Handles a bonus event, return true if handled
		
		virtual unsigned long getExJob(double time) = 0;					// Returns the job-ID of the current job under execution
		virtual Event * nextDeparture(double speed, double time) = 0;		// Creates a new departure Event object and retunrs its pointer
		virtual Event * nextScheduler(double speed, double time) = 0;		// Creates a new scheduler event, place holder for unknown desings

		bool isOverwritten(DepartureEvent *e)	// Returns true if the event is overwritten
		{return (e->validId_m != validDepartureEvent_m);}

		bool isOverwritten(SchedulerEvent *e)	// Returns true if the event is overwritten
		{return (e->validId_m != validSchedulerEvent_m);}

		virtual void updatePeriod(double time1, double time2, double speed, DESLogger *, PowerFunction *) = 0;	/* Applies the passage of time.
																												 - Parameter one (time1) should match the 
																												internal value of previous update time. 
																												- Parameter three (speed), gives the speed 
																												during interval [time1, time2]
																												 - DESLogger * is used for simulation logs 
																												 - Must call DESLogger.execution_handler*/
	protected:
		double lastUpdate_m;					// Keeps the time of the end of last updatePeriod
		unsigned long validDepartureEvent_m;	// Kept for departure event overwrite checks	
		unsigned long validSchedulerEvent_m;	// Kept for scheduler event overwrite checks, should always be even, initialize to zero	

	public:
		unsigned long nextValidDepartureID()			// Returns the next valid departure id
		{	if (validDepartureEvent_m <= UINT_MAX - 2)
				return ++validDepartureEvent_m;
			else
				return validDepartureEvent_m = 0;}
		unsigned long nextValidSchedulerID()			// Returns the next valid departure id, should always be even
		{	if (validSchedulerEvent_m <= UINT_MAX - 3) 
				validSchedulerEvent_m += 2;
			else
				validSchedulerEvent_m = 0;
			return validSchedulerEvent_m;}
	};

//----------------------------------------------------------------------

#endif