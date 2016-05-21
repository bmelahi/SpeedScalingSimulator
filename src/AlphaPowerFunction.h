
#ifndef _AlphaPowerFunction_h_

	#define _AlphaPowerFunction_h_

/*----------------------------------------------------------------------
*AlphaPowerFunction.h - Implementation of the abstract class PowerFunction
*
*       Written by Maryam Elahi
*		Last updated: Nob, 2012
*
*Purpose:
*       This file defines the AlphaPowerFunction with its members and methods.
*
*///--------------------------------------------------------------------

	#include "PowerFunction.h"
	#include "GlobalsAndTypes.h"
		
//----------------------------------------------------------------------

	class AlphaPowerFunction : public PowerFunction {
	public:
		AlphaPowerFunction(double alpha) : alpha_m(alpha) {}

	public:
		double function(double speed);
		double inverseFunction(double N);

		double getAlpha() {return alpha_m;}

	private:
		double alpha_m;
	};

//----------------------------------------------------------------------
//----------------------------------------------------------------------


#endif