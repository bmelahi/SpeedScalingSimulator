/***
* GlobalsAndTypes.cpp - implementation of global const functions
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov, 2012
*
* Purpose:
*       This file contains the implementation of global const functions.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "GlobalsAndTypes.h"

using namespace std;
	
//----------------------------------------------------------------------

/* Constant functions */
	
/* Functions for handling comparison of type double.
	The Knuth algorithm for close enough with tolerance DOUBLE_PRECISION is used.
*/
const bool approximatelyEqual(const double a, const double b) {
	if (fabs(a) < 10 && fabs(b) < 10) 
		return fabs(a - b) <= LOOSE_DOUBLE_PRECISION_EPSILON; 
	else
		//return fabs(a - b) <= LOOSE_DOUBLE_PRECISION_EPSILON; 
		return ((fabs(a - b) / DOUBLE_PRECISION_EPSILON) <= fabs(a) || (fabs(a - b) / DOUBLE_PRECISION_EPSILON) <= fabs(b));

	/* In case of the values being zero, the modified boost version should be used? */
	//if (a != 0 && b !=0)
	//	return (abs(a-b) <= DOUBLE_PRECISION / abs(a) || abs(a-b) <= DOUBLE_PRECISION / abs(b));
	//else
	//	return (abs(a-b) <= DOUBLE_PRECISION * abs(a) || abs(a-b) <= DOUBLE_PRECISION * abs(b));
}

const bool definitelyGreaterThan(const double a, const double b) {
	if (approximatelyEqual(a, b))
		return false;
	else
		return a > b;
	//if (approximatelyEqual(a - b, 0))
	//	return false;
	//else
	//	return (a - b) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * DOUBLE_PRECISION_EPSILON);
}

const bool definitelyLessThan(const double a, const double b) {
	if (approximatelyEqual(a, b))
		return false;
	else
		return a < b;
	//if (approximatelyEqual(a - b, 0))
	//	return false;
	//return (b - a) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * DOUBLE_PRECISION_EPSILON);
}
	
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
