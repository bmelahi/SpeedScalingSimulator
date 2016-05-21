
#ifndef _WLGenPareto_h_

	#define _WLGenPareto_h_

/*----------------------------------------------------------------------
* WLGenPareto.h - definitions/declarations for class WLGenPareto
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Dec 2012
*
* Purpose:
*       This file defines the class WLGenPareto, with its members and methods.
*
*///---------------------------------------------------------------------
	
	#include <string>	
	#include <fstream>
	
	#include "Event.h"
	#include "GlobalsAndTypes.h"
	#include "WorkLoadGenerator.h"

//----------------------------------------------------------------------

	class WLGenPareto: public WorkloadGenerator {
	public:
		WLGenPareto(double arrivalRate, double alpha, unsigned long maxN);
		
		virtual ~WLGenPareto();

	public:
		std::string toString();			// Returns the name and parameters of the object

		Event * next_arrival();			// Creates a new arrival Event object and retunrs its pointer

	private:
		double rateArr_m;
		double alpha_m;
		double load_m;

		double lastTime_m;				// For arrival time generation, the random arrival time is added to this time.

	private:
		static std::string name_s;
	};

//----------------------------------------------------------------------

#endif