/***
* Job.cpp - implementation of class Job
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file contains the implementation of the class Job.
*
****/

//----------------------------------------------------------------------

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include "Job.h"
#include "yLog.h"
	
//----------------------------------------------------------------------

bool Job::operator<(const Job & j) const {
	if (definitelyLessThan(arrival_m,j.arrival_m))
		return true;
	else
		return false;
}

//----------------------------------------------------------------------

void Job::set_probe(unsigned long probe_id) {
	isProbe_m = true;
	probeid_m = probe_id;
}

//----------------------------------------------------------------------

bool jobPointerSort(const Job * j1, const Job * j2) {		
	return definitelyGreaterThan(j1->getSize(), j2->getSize()); 
}


//----------------------------------------------------------------------
