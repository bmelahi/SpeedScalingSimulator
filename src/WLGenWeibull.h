#ifndef _WLGenWeibull_h_

	#define _WLGenWeibull_h_

/*----------------------------------------------------------------------
* WLGenWeibull.h - definitions/declarations for class WLGenWeibull
*
*       Written by Mithun P
*		Last (remembered) updated: Aug 2022
*
* Purpose:
*       This file defines the class WLGenWeibull, with its members and methods.
*
*
*///---------------------------------------------------------------------

#include <string>	
#include <fstream>

#include "Event.h"
#include "GlobalsAndTypes.h"
#include "WorkLoadGenerator.h"

//----------------------------------------------------------------------

class WLGenWeibull : public WorkloadGenerator {
public:
	WLGenWeibull(double load, double alpha, double beta, double mean, double sd, unsigned long maxN);
	// WLGenWeibull(double arrivalRate, double alpha,double sd, unsigned long maxN);
	// WLGenWeibull(double arrivalRate, unsigned long maxN);
	virtual ~WLGenWeibull();

public:
	std::string toString();			// Returns the name and parameters of the object

	Event* next_arrival();			// Creates a new arrival Event object and retunrs its pointer

public:
	double factorial(double value);

private:
	double rateArr_m;
	double alpha_m;
	double beta_m;
	double mean_m;
	double sd_m;
	double load_m;
	double weibullMean_m;
	// bool flipSign;

	double lastTime_m;				// For arrival time generation, the random arrival time is added to this time.

private:
	static std::string name_s;
};

//----------------------------------------------------------------------

#endif