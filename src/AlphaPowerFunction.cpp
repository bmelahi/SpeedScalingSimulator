/***
* AlphaPowerFunction - implementation of class AlphaPowerFunction
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file contains the implementation of the class AlphaPowerFunction.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include <cmath>

#include "AlphaPowerFunction.h"
	
//----------------------------------------------------------------------

double AlphaPowerFunction::function(double speed) {
	if (alpha_m <= 0)
		return pow(speed, 1);
	else
		return pow(speed, alpha_m);
}

//----------------------------------------------------------------------

double AlphaPowerFunction::inverseFunction(double N) {
	if (alpha_m <= 0)
		//return pow(N, 1);
		return 1.0;
	else
		return pow(N, 1/alpha_m);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
