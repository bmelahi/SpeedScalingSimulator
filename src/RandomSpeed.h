
#ifndef _RandomSpeed_h_

	#define _RandomSpeed_h_

/*----------------------------------------------------------------------
* RandomSpeed.h - definitions/declarations for class RandomSpeed
*
*       Written by Maryam Elahi
*		Last (remembered) updated: March 2013
*
*Purpose:
*       This file defines the class RandomSpeed, which uses 
*		the inverse of the power fuction to set the speed. 
*
*///---------------------------------------------------------------------

	#include "Event.h"
	#include "Scheduler.h"
	#include "SpeedScaler.h"
	#include "DESLogger.h"
	#include "PowerFunction.h"
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class RandomSpeed: public SpeedScaler {
	public:
		RandomSpeed(double base, PowerFunction * P);
		~RandomSpeed();

	public:
		std::string toString();		// Returns the name and parameters of the object

		bool arrival_handler(ArrivalEvent *);				// Handles an arrival event, return true if speed changes
		bool departure_handler(DepartureEvent *);			// Handles a departure event, return true if speed changes
		bool speedchange_handler(SpeedChangeEvent * e);		// Handles a speed-change event, return true if speed changes
		Event * bonusevent_handler(SchedulerEvent *);		// Handles a bonus event, return true if handled
		
		double getExSpeed(double time);						// Returns the execution speed at time. Parameter should match the internal time.
		Event * nextSpeedchange(double time);				// Creates a new speed_change Event object and retunrs its pointer
		Event * nextScheduler(double speed, double time);	// Creates a new scheduler event, place holder for unknown desings
		
		void updatePeriod(double time1, double time2, DESLogger *);			/* Applies the passage of time.
																			   - Parameter one (time1) should match the 
																			   internal value of previous update time. 
																			   - DESLogger * is used for simulation logs */
	private:
		SpeedChangeEvent * nextSpeedChangeEvent_m;
		unsigned long jobCount_m;

		double speed_m;

		double speedMean_m;
		double intervalMean_m;

	private:
		static std::string name_s;

	};

//----------------------------------------------------------------------

#endif