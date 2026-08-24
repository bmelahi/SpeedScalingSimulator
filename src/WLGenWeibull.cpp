/***
* WLGenWeibull.cpp - implementation of WorkloadFileWrapper WorkLoadGenerator class
*
*       Written by Mithun P
*		Last (remembered) updated: Aug, 2022
*
* Purpose:
*       This file contains the implementation of the Weibull WorkLoadGenerator class.
*
****/

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "yLog.h"
#include "Event.h"
#include "GlobalsAndTypes.h"
#include "WLGenWeibull.h"
#include "PRandomGen.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>


using namespace std;

//----------------------------------------------------------------------

// Object's name
string WLGenWeibull::name_s = "WLGenWeibull";

//----------------------------------------------------------------------

WLGenWeibull::WLGenWeibull(double load, double alpha, double beta, double mean, double sd, unsigned long maxN)
	: load_m(load), alpha_m(alpha), beta_m(beta),mean_m(mean),sd_m(sd), lastTime_m(0) {

	maxN_m = maxN;

	// Calculating Arrival Rate based on the load
	//weibullMean_m = beta_m * factorial(1/alpha_m);
	//rateArr_m = load_m / weibullMean_m; 

	// TO FIX : 
	// Assuming beta_m gives us the weibullMean_m
	weibullMean_m = beta_m;
	beta_m = weibullMean_m / factorial(1 / alpha_m);
	rateArr_m = load_m / weibullMean_m; 
	
	// PRandomGen::reset();
	PRandomGen::setGeneratorWeibull(alpha_m, beta_m, mean_m, sd_m); //Sets the parameters for the logNormal and Weibull generators

	char filename[200];
	sprintf(filename, "WLPTR--ArrRate %f -Alpha %f -Beta %f -Mean %f -Sd %f.dat", rateArr_m, alpha_m, beta_m,mean_m, sd_m);

	 if (rateArr_m <= 0 || alpha_m <= 0 || load_m <= 0 || beta_m <= 0 || sd_m <= 0)
		yLog::logtime(ERRORLOG, __FUNCTION__, "Illegal initialization with zero or negative values! %s", filename);
}

//----------------------------------------------------------------------

WLGenWeibull::~WLGenWeibull() {
	// Anything to do?
}

//----------------------------------------------------------------------

// Returns the name and parameters of the object
string WLGenWeibull::toString() {
	stringstream ss;
	ss << "(" << name_s << " ArrRate=" << rateArr_m << " Alpha= " << alpha_m << " beta= " << beta_m << " mean= " << mean_m << " Std Dev= " << sd_m << ")";
	return ss.str();
}

//----------------------------------------------------------------------

// Creates a new arrival Event object and returns its pointer
Event* WLGenWeibull::next_arrival() {
	double deadline = 0;
	unsigned id = ++lastID_m;

	double time = lastTime_m;
	double size = PRandomGen::getWeibull();
	lastTime_m += PRandomGen::getExponential(1 / rateArr_m);
	double multiplier = PRandomGen::getLogNormal();// The value that size is going to be multiplied by
												   // to produce an estimation error

	// Size estimation
	double estimated = size;
	estimated *= multiplier;

	// END Size Estimation

	return new ArrivalEvent(time, Event_Type::ARRIVAL, id, Job(time, id, size, deadline, estimated), 0);
}

//----------------------------------------------------------------------

double WLGenWeibull::factorial(double value) {
	return std::tgamma(value + 1);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------