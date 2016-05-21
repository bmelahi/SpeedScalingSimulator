
#ifndef _WorkloadGenerator_h_

	#define _WorkloadGenerator_h_

/*----------------------------------------------------------------------
* WorkloadGenerator.h - definitions/declarations for class WorkloadGenerator
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
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class WorkloadGenerator {
	public:
		WorkloadGenerator() 
			: invalidID_m(JobID_Type::INVALIDID), 
			  probeID_m(JobID_Type::PROBEIDBASE), 
			  firstID_m(JobID_Type::JOBIDBASE),
			  lastID_m(JobID_Type::JOBIDBASE - 1),
			  maxN_m(0)
		{}
		
		virtual ~WorkloadGenerator() = 0 {}

	public:
		virtual std::string toString() = 0;		// Returns the name and parameters of the object

		virtual Event * next_arrival() = 0;			// Creates a new arrival Event object and retunrs its pointer
		unsigned long getMaxN() {return maxN_m;}

	protected:
		unsigned long invalidID_m;		// Base invalid ID
		unsigned long probeID_m;		// Base probe ID
		unsigned long firstID_m;		// Base job ID
		unsigned long lastID_m;			// Last job ID produced
		unsigned long maxN_m;			// The last job ID of interest
	};

//----------------------------------------------------------------------

#endif