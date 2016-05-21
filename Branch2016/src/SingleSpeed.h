
#ifndef _SingleSpeed_h_

	#define _SingleSpeed_h_

/*----------------------------------------------------------------------
* SpeedScaler.h - definitions/declarations for class SingleSpeed
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file defines the class SingleSpeed, which uses a constant 
*		speed received in its constructor. The speed may be changed 
*		by expternal call.
*
*///---------------------------------------------------------------------

	#include "Event.h"
	#include "SpeedScaler.h"
	#include "DESLogger.h"
	#include "PowerFunction.h"	
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class SingleSpeed: public SpeedScaler {
	public:
		SingleSpeed(double speed, double base, PowerFunction * P);
		~SingleSpeed();

	public:
		std::string toString();		// Returns the name and parameters of the object

		bool arrival_handler(ArrivalEvent *);					// Handles an arrival event, return true if speed changes
		bool departure_handler(DepartureEvent *);				// Handles a departure event, return true if speed changes
		bool speedchange_handler(SpeedChangeEvent *);	// Handles a speed-change event, return true if speed changes
		Event * bonusevent_handler(SchedulerEvent *);			// Handles a bonus event, return true if handled
		
		double getExSpeed(double time);						// Returns the execution speed at time. Parameter should match the internal time.
		Event * nextSpeedchange(double time);				// Creates a new speed_change Event object and retunrs its pointer
		Event * nextScheduler(double speed, double time);	// Creates a new scheduler event, place holder for unknown desings
		
		void updatePeriod(double time1, double time2, DESLogger *);			/* Applies the passage of time.
																			   - Parameter one (time1) should match the 
																			   internal value of previous update time. 
																			   - DESLogger * is used for simulation logs */
	private:
		double speed_m;				// The base speed
		unsigned long jobCount_m;		// If zero, speed is gated to zero

	//public:
	//	void changeSpeed(double speed);

	private:
		static std::string name_s;
	};

//----------------------------------------------------------------------

#endif