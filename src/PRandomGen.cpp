/***
* PRandomGen.cpp - implementation of class PRandomGen
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file contains the implementation of the class for a 
*		generation of random numbers with different distributions.
*
****/

//----------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>
#include <random>

#include "GlobalsAndTypes.h"
#include "PRandomGen.h"
#include "yLog.h"

using namespace std;


//----------------------------------------------------------------------

bool PRandomGen::isFromFile_m = false;
FILE * PRandomGen::fin = NULL;
FILE * PRandomGen::PRAND_s = NULL;
unsigned int PRandomGen::seed_m = 1982;				// Is initialized if there is a seed.
unsigned int PRandomGen::MAX_RAND_m = RAND_MAX + 1;			// The maximum random number


std::default_random_engine generator;
std::uniform_real_distribution<double> distributionUnf(0.0, 1.0);
std::weibull_distribution<double> PRandomGen::distributionWeibull_m;
std::lognormal_distribution<double> PRandomGen::distributionLogNormal_m;
std::default_random_engine PRandomGen::generatorWeibull_m;
std::default_random_engine PRandomGen::generatorLogNormal_m;
std::uniform_real_distribution<double> PRandomGen::distributionUninform_m;

// std::lognormal_distribution<double> distributionLog(0.0, 0.5);

//----------------------------------------------------------------------

// Gets the random numbers from the random bytes file
void PRandomGen::setupGen (string randombytefile) {
	isFromFile_m = true;

	fin = fopen(randombytefile.c_str(), "rb");
    if (fin == NULL) {
		cerr << "Could not open the random-bytes file!" << endl;
		yLog::logtime(ERRORLOG, __FUNCTION__, "Could not open the random-bytes file! %s", randombytefile.c_str());
		exit(0);
	}

	// FIXME
	char filename[200];
	sprintf(filename, "PRANDNumbers.dat");
	PRAND_s = fopen(filename, "wt");

	//MAX_RAND_m = UINT_MAX;
	// END FIXME
}

//----------------------------------------------------------------------

// Gets the random numbers from the rand(), with the given seed 
void PRandomGen::setupGen (unsigned int seed) {
	isFromFile_m = false;
	seed_m = seed;
	srand(seed_m);
	MAX_RAND_m = RAND_MAX + 1;

	// FIXME
	char filename[200];
	sprintf(filename, "PRANDNumbers.dat");
	PRAND_s = fopen(filename, "wt");
	// END FIXME
}

//----------------------------------------------------------------------
//Sets the distribution parameters for a weibull distribution with log normal estimations
void PRandomGen::setGeneratorWeibull(double alpha, double beta, double mean, double sd) {
	distributionWeibull_m = std::weibull_distribution<double> (alpha, beta);
	distributionLogNormal_m = std::lognormal_distribution<double> (mean, sd);
	std::weibull_distribution<double>::param_type p = distributionWeibull_m.param();
	std::lognormal_distribution<double>::param_type q = distributionLogNormal_m.param();
	printf("%f %f\n", p.a(), p.b());
	printf("%f %f\n", q.m(), q.s());
}
//----------------------------------------------------------------------

//

PRandomGen::~PRandomGen() {
	if (isFromFile_m)
		fclose(fin);
}

//----------------------------------------------------------------------

// Returns the next random number in [2^0 2^32]
unsigned int PRandomGen::getUrand() {
	unsigned ret = 0;
	if (isFromFile_m) {
		// FIXME
		short twobytes = 0;
		size_t b = fread(&twobytes, sizeof(twobytes), 1, fin);
		//ret = twobytes & 0x7FFF;
		ret = abs(twobytes);
		// End FIXME
		//size_t b = fread(&ret, sizeof(ret), 1, fin);
		if (b < 1) {
			cerr << "Could not read the random-bytes file!" << endl;
			exit(0);
		}
	}
	else
		ret = rand();

	// FIXME
	//fprintf(PRAND_s, "%20d\n", ret);
	// FIXME
	return ret;
}

//----------------------------------------------------------------------

// Resets the generator, if from file rewinds, if from rand() reseeds
void PRandomGen::reset() {
	if (isFromFile_m) 
		rewind(fin);
	else
		srand(seed_m);

	// FIXME
	char filename[200];
	sprintf(filename, "PRANDNumbers.dat");
	PRAND_s = fopen(filename, "wt");
	// END FIXME
}

//----------------------------------------------------------------------

// Returns the next uniformely random number in [0, 1]
double PRandomGen::getUrandFrac() {
	return ((double)getUrand() / (double)MAX_RAND_m); 
}

//----------------------------------------------------------------------

// Returns the next uniformely random number in [r1, r2)
unsigned int PRandomGen::getUrandRange(unsigned int r1, unsigned int r2) {
	return (getUrand() % (r2 - r1) + r1); 
}

//----------------------------------------------------------------------

// Retunrs the next random number from an exponential dist with mean
double PRandomGen::getExponential(double mean) {
	double rnd = getUrandFrac();
	while (rnd == 1 || rnd == 0) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "getUrandFrac returned %f", rnd);
		yLog::logtime(DEBUGLOG, __FUNCTION__, "getUrandFrac returned %f", rnd);
		rnd = getUrandFrac();
	}

	if (mean == 0) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Zero mean!");
		return 0;
	}

	return (-1.0) * mean * log(rnd);
}

//----------------------------------------------------------------------

// Retunrs the next random number from an pareto dist with alpha
double PRandomGen::getPareto(double alpha) {
	//double rnd = getUrandFrac();
	double rnd = distributionUninform_m(generator);
	while (rnd == 1 || rnd == 0) {
		//yLog::logtime(ERRORLOG, __FUNCTION__, "getUrandFrac returned %f", rnd);
		yLog::logtime(DEBUGLOG, __FUNCTION__, "getUrandFrac returned %f", rnd);
		rnd = getUrandFrac();
	}

	if (alpha == 0) {
		yLog::logtime(ERRORLOG, __FUNCTION__, "Zero alpha!");
		return 0;
	}
	double xm = (alpha - 1) / alpha;
	return xm / pow(rnd, 1.0 / alpha);
}

//----------------------------------------------------------------------

// Returns the next random number from an Weibull dist with alpha(shape) and beta(scale)
double PRandomGen::getWeibull() {
	//double rnd = getUrandFrac();
	double rnd = distributionWeibull_m(generatorWeibull_m);
	while (rnd == 1 || rnd == 0) {
		//yLog::logtime(ERRORLOG, __FUNCTION__, "getUrandFrac returned %f", rnd);
		yLog::logtime(DEBUGLOG, __FUNCTION__, "getUrandFrac returned %f", rnd);
		rnd = getUrandFrac();
	}

	//if (a == 0 || b == 0) {
	//	yLog::logtime(ERRORLOG, __FUNCTION__, "Zero a or Zero b!");
	//	return 0;
	//}

	// if scale parameter b is set as 1, then shape parameter can be varied accordingly
	// double xm = ((a / b) * pow(rnd / b, a - 1) * exp(-1 * pow(rnd / b, a)));
	// return xm;

	return rnd;

}

//----------------------------------------------------------------------

// Returns the next random number from an LogNormal dist with mean and sd
double PRandomGen::getLogNormal() {
	//double rnd = getUrandFrac();
	double rnd = distributionLogNormal_m(generatorLogNormal_m);
	while (rnd == 1 || rnd == 0) {
		//yLog::logtime(ERRORLOG, __FUNCTION__, "getUrandFrac returned %f", rnd);
		yLog::logtime(DEBUGLOG, __FUNCTION__, "getUrandFrac returned %f", rnd);
		rnd = getUrandFrac();
	}

	//if (sd == 0) {
	//	yLog::logtime(ERRORLOG, __FUNCTION__, "Zero log!");
	//	return 0;
	//}
	// double xm = 1 / (sd * rnd * pow(2 * M_PI, 1 / 2)) * exp(-1 * (pow(log(rnd) - mean, 2)) / (2 * sd * sd));
	return rnd;
}
//----------------------------------------------------------------------