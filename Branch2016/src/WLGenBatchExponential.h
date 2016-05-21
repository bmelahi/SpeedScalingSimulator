
#ifndef _WLGenBatchExponential_h_

	#define _WLGenBatchExponential_h_

/*----------------------------------------------------------------------
* WLGenBatchExponential.h - definitions/declarations for class WLGenBatchExponential
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Oct 2013
*
* Purpose:
*       This file defines the class WLGenBatchExponential, with its members and methods.
*
*///---------------------------------------------------------------------
	
	#include <string>	
	#include <fstream>
	
	#include "Event.h"
	#include "GlobalsAndTypes.h"
	#include "WorkLoadGenerator.h"

//----------------------------------------------------------------------

	class WLGenBatchExponential: public WorkloadGenerator {
	public:
		WLGenBatchExponential(double meanBatchCount, double meanBatchInterval, double meanSize, unsigned long maxN);
		
		virtual ~WLGenBatchExponential();

	public:
		std::string toString();			// Returns the name and parameters of the object

		Event * next_arrival();			// Creates a new arrival Event object and retunrs its pointer

	private:
		double meanBatchCount_m;
		double meanBatchInterval_m;
		double meanSize_m;
		double load_m;

		double lastTime_m;				// For arrival time generation, the random arrival time is added to this time.
		double lastCount_m;				// For Batch job generation count
		double currentRandomCount_m;	// For each batch, the random number of jobs in the batch is keept in this variable.

	private:
		static std::string name_s;
	};

//----------------------------------------------------------------------

#endif