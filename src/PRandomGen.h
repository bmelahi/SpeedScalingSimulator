
#ifndef _PRandomGen_h_

	#define _PRandomGen_h_

/*----------------------------------------------------------------------
* PRandomGen.h - definitions/declarations for abstract class PRandomGen
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
* Purpose:
*       This file defines the abstract class PRandomGen, with its members and methods.
*
*///---------------------------------------------------------------------

	#include <cmath>
	#include <cstdlib>
	#include <cstdio>
	#include <iostream>
	#include <random>
//----------------------------------------------------------------------

	class PRandomGen {
	public:
		PRandomGen () {}
		~PRandomGen ();

	public:
		static void setupGen(std::string randombytefile);		// Gets the random numbers from the random bytes file
		static void setupGen(unsigned int seed);				// Gets the random numbers from the rand()  
		static void setGeneratorWeibull(double alpha, double beta, double mean, double sd); // Sets the parameters for the 
																							// LogNormal and Weibull generators

	public:
		static unsigned int getUrand();			// Returns the next random number in [2^0 2^32)
		static double getUrandFrac();				// Returns the next uniformely random number in [0, 1)
		static unsigned int getUrandRange(unsigned int r1, unsigned int r2);	// Returns the next uniformely random number in [r1, r2)

		static double getExponential(double mean);	// Returns the next random number from an exponential dist with mean
		static double getPareto(double alpha);		// Returns the next random number from an pareto dist with alpha
		static double getLogNormal();	// Returns the next random number from an LogNormal dist with log
		static double getWeibull();		// Returns the next random number from an Weibull dist with alpha
		static void reset();						// Resets the generator, if from file rewinds, if from rand() reseeds

	private:
		static bool isFromFile_m;
		static FILE * fin;
		static unsigned int seed_m;				// Is initialized if there is a seed.
		static unsigned int MAX_RAND_m;			// The maximum for the random number

		static FILE * PRAND_s;

		 static std::default_random_engine generatorLogNormal_m;
		 static std::default_random_engine generatorWeibull_m;
		 static std::default_random_engine generatorPareto_m;

		 static std::lognormal_distribution<double> distributionLogNormal_m;
		 static std::weibull_distribution<double> distributionWeibull_m;
		 static std::uniform_real_distribution<double> distributionUninform_m;
	};

//----------------------------------------------------------------------

#endif