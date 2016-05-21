
#ifndef _WLGenExponential_h_

	#define _WLGenExponential_h_

/*----------------------------------------------------------------------
* WLGenExponential.h - definitions/declarations for class WLGenExponential
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
* Purpose:
*       This file defines the class WLGenExponential, with its members and methods.
*
*///---------------------------------------------------------------------
	
	#include <string>	
	#include <fstream>
	
	#include "Event.h"
	#include "GlobalsAndTypes.h"
	#include "WorkLoadGenerator.h"

//----------------------------------------------------------------------

	class WLGenExponential: public WorkloadGenerator {
	public:
		WLGenExponential(double arrivalRate, double meanSize, unsigned long maxN);
		
		virtual ~WLGenExponential();

	public:
		std::string toString();			// Returns the name and parameters of the object

		Event * next_arrival();			// Creates a new arrival Event object and retunrs its pointer

	private:
		double rateArr_m;
		double meanSize_m;
		double load_m;

		double lastTime_m;				// For arrival time generation, the random arrival time is added to this time.

	private:
		static std::string name_s;
		std::ofstream WLG_s;
	};

//----------------------------------------------------------------------

#endif