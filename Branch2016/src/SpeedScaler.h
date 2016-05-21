
#ifndef _SpeedScaler_h_

	#define _SpeedScaler_h_

/*----------------------------------------------------------------------
* SpeedScaler.h - definitions/declarations for class SpeedScaler
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file defines the class SpeedScaler, with its members and methods.
*
*///---------------------------------------------------------------------

	#include "Event.h"
	#include "DESLogger.h"
	#include "PowerFunction.h"
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class SpeedScaler {
	public:
		SpeedScaler(double base, PowerFunction * P) 
			: lastUpdate_m(0), validSpeedChangeEvent_m(0), validSchedulerEvent_m(1), baseSpeed_m(base), P_m(P){}	
		virtual ~SpeedScaler() = 0 {delete P_m;}

	public:
		virtual std::string toString() = 0;		// Returns the name and parameters of the object

		virtual bool arrival_handler(ArrivalEvent *) = 0;					// Handles an arrival event, return true if speed changes
		virtual bool departure_handler(DepartureEvent *) = 0;				// Handles a departure event, return true if speed changes
		virtual bool speedchange_handler(SpeedChangeEvent *) = 0;			// Handles a speed-change event, return true if speed changes
		virtual Event * bonusevent_handler(SchedulerEvent *) = 0;			// Handles a bonus event, return true if handled
		
		virtual double getExSpeed(double time) = 0;							// Returns the execution speed at time. Parameter should match the internal time.
		virtual Event * nextSpeedchange(double time) = 0;					// Creates a new speed_change Event object and retunrs its pointer
		virtual Event * nextScheduler(double speed, double time) = 0;		// Creates a new scheduler event, place holder for unknown desings
		
		bool isOverwritten(SpeedChangeEvent *e)			// Returns true if the event is overwritten
		{return (e->validId_m != validSpeedChangeEvent_m);}
		bool isOverwritten(SchedulerEvent *e)			// Returns true if the event is overwritten
		{return (e->validId_m != validSchedulerEvent_m);}


		virtual void updatePeriod(double time1, double time2, DESLogger *) = 0;		/* Applies the passage of time.
																					   - Parameter one (time1) should match the 
																					   internal value of previous update time. 
																					   - DESLogger * is used for simulation logs */
	protected:
		double lastUpdate_m;					// Keeps the time of the end of last updatePeriod
		unsigned long validSpeedChangeEvent_m;	// Kept for SpeedChange event overwrite checks	
		unsigned long validSchedulerEvent_m;	// Kept for SpeedChange event overwrite checks, should always be odd, initialize to 1	
		PowerFunction * P_m;					// Point to the power function
		double baseSpeed_m;						// Base speed, the current speed will be scaled by this value on return. Default 1.

	public:
		unsigned nextValidSpeedChangeID()		// Returns the next valid departure id
		{	if (validSpeedChangeEvent_m <= UINT_MAX - 2)
				return ++validSpeedChangeEvent_m;
			else
				return validSpeedChangeEvent_m = 0;}
		unsigned nextValidSchedulerID()			// Returns the next valid departure id, Should always be odd numbers
		{	if (validSchedulerEvent_m <= UINT_MAX - 3) 
				validSchedulerEvent_m += 2;
			else
				validSchedulerEvent_m = 1;
			return validSchedulerEvent_m;}
	};

//----------------------------------------------------------------------

#endif