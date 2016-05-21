
#ifndef _WLGenCustom_h_

	#define _WLGenCustom_h_

/*----------------------------------------------------------------------
* WLGenCustom.h - definitions/declarations for class WLGenCustom
*
*       Written by Maryam Elahi
*		Last (remembered) updated: March 2013
*
* Purpose:
*       This file defines the class WLGenCustom, with its members and methods.
*
*///---------------------------------------------------------------------
	
	#include <string>	
	#include <fstream>
	
	#include "Event.h"
	#include "GlobalsAndTypes.h"
	#include "WorkLoadGenerator.h"

//----------------------------------------------------------------------

	class WLGenCustom: public WorkloadGenerator {
	public:
		WLGenCustom(double arrivalRate, double meanSize, unsigned long maxN);
		
		virtual ~WLGenCustom();

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
	};

//----------------------------------------------------------------------

#endif