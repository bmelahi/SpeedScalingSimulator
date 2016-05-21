
#ifndef _PowerFunction_h_

	#define _PowerFunction_h_

/*----------------------------------------------------------------------
*PowerFunction.h - definitions/declarations for abstract class PowerFunction
*
*       Written by Maryam Elahi
*		Last updated: Nob, 2012
*
*Purpose:
*       This file defines the abstract class PowerFunction with its members and methods.
*
*///--------------------------------------------------------------------

	#include "GlobalsAndTypes.h"
		
//----------------------------------------------------------------------

	class PowerFunction {
	public:
		virtual double function(double speed) = 0;			// Returns P(speed)
		virtual double inverseFunction(double) = 0;			// Returns the P^-1(x)
	};

//----------------------------------------------------------------------
//----------------------------------------------------------------------


#endif