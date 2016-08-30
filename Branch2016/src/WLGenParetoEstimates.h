
#ifndef _WLGenParetoEstimates_h_

	#define _WLGenParetoEstimates_h_

/*----------------------------------------------------------------------
* WLGenParetoEstimates.h - definitions/declarations for class WLGenParetoEstimates
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Dec 2012
*
* Purpose:
*       This file defines the class WLGenParetoEstimates, with its members and methods.
*
*///---------------------------------------------------------------------
	
	#include <string>	
	#include <fstream>
	
	#include "Event.h"
	#include "GlobalsAndTypes.h"
	#include "WorkLoadGenerator.h"

//----------------------------------------------------------------------

	class WLGenParetoEstimates: public WorkloadGenerator {
	public:
		WLGenParetoEstimates(double arrivalRate, double alpha, unsigned long maxN);
		
		virtual ~WLGenParetoEstimates();

	public:
		std::string toString();			// Returns the name and parameters of the object

		Event * next_arrival();			// Creates a new arrival Event object and retunrs its pointer

	private:
		double rateArr_m;
		double alpha_m;
		double load_m;
		bool flipSign;

		double lastTime_m;				// For arrival time generation, the random arrival time is added to this time.

	private:
		static std::string name_s;
	};

//----------------------------------------------------------------------

#endif