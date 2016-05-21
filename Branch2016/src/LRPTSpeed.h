
#ifndef _LRPTSpeed_h_

	#define _LRPTSpeed_h_

/*----------------------------------------------------------------------
* LRPTSpeed.h - definitions/declarations for class LRPTSpeed
*
*       Written by Maryam Elahi
*		Last (remembered) updated: March 2013
*
*Purpose:
*       This file defines the class LRPTSpeed, which uses 
*		the inverse of the power fuction to set the speed. 
*
*///---------------------------------------------------------------------

	#include "Event.h"
	#include "SpeedScaler.h"
	#include "DESLogger.h"
	#include "PowerFunction.h"
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class LRPTSpeed: public SpeedScaler {
	public:
		LRPTSpeed(double base, PowerFunction * P);
		~LRPTSpeed();

	public:
		std::string toString();		// Returns the name and parameters of the object

		bool arrival_handler(ArrivalEvent *);				// Handles an arrival event, return true if speed changes
		bool departure_handler(DepartureEvent *);			// Handles a departure event, return true if speed changes
		bool speedchange_handler(SpeedChangeEvent *);		// Handles a speed-change event, return true if speed changes
		Event * bonusevent_handler(SchedulerEvent *);			// Handles a bonus event, return true if handled
		
		double getExSpeed(double time);						// Returns the execution speed at time. Parameter should match the internal time.
		Event * nextSpeedchange(double time);				// Creates a new speed_change Event object and retunrs its pointer
		Event * nextScheduler(double speed, double time);	// Creates a new scheduler event, place holder for unknown desings
		
		void updatePeriod(double time1, double time2, DESLogger *);			/* Applies the passage of time.
																			   - Parameter one (time1) should match the 
																			   internal value of previous update time. 
																			   - DESLogger * is used for simulation logs */
	private:
		unsigned long jobCount_m;		// If zero, speed is gated to zero
		unsigned long LRPTJC_m;			// Keeps adding up with arrivals, till the jobCount_m become zero.

	private:
		static std::string name_s;
	};

//----------------------------------------------------------------------

#endif