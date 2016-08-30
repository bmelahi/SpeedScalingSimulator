
#ifndef _PSEE_h_

	#define _PSE_h_

/*----------------------------------------------------------------------
* PSE.h - definitions/declarations for class PSE
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file defines the class PSE, with its members and methods.
*
*///---------------------------------------------------------------------

	#include <set>
	#include <string>

	#include "PS.h"
	#include "Event.h"
	#include "DESLogger.h"
	#include "Scheduler.h"
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

class PSE : public PS {
public:
	PSE();
	~PSE();

public:
	std::string toString();		// Returns the name and parameters of the object

public:
	void clearQueue(double time);		// Clear the remaining work in the queue update the time and the validID.

	private:
		static string name_s;
	};

//----------------------------------------------------------------------

#endif