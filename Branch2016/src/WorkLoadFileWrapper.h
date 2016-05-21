
#ifndef _WorkloadFileWrapper_h_

	#define _WorkloadFileWrapper_h_

/*----------------------------------------------------------------------
* WorkloadGenerator.h - definitions/declarations for class WorkloadGenerator
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
* Purpose:
*       This file defines the class Scheduler, with its members and methods.
*
*///---------------------------------------------------------------------
	
	#include <string>	
	#include <fstream>
	
	#include "Event.h"
	#include "GlobalsAndTypes.h"
	#include "WorkLoadGenerator.h"

//----------------------------------------------------------------------

	class WorkloadFileWrapper: public WorkloadGenerator {
	public:
		WorkloadFileWrapper(std::string filename, unsigned long maxN);
		~WorkloadFileWrapper();

	public:
		std::string toString();			// Returns the name and parameters of the object

		Event * next_arrival();			// Creates a new arrival Event object and retunrs its pointer
		//Event * next_probe();			// Creates a new arrival Event object for a probe job and retunrs its pointer
		//bool isProbe(unsigned long);		// Returns true, if the input parameter is a probe job id
		//bool isValidJobID(unsigned long);	// Returns true, if the input parameter is a valid job id already produced
		//bool insertProbe(double time, double size);					/* Returns true, if the insertion of probe 
		//														       within the limit of the workload is possible */


	private:
		std::string filename_m;
		std::ifstream wlfile_m;

	private:
		static string name_s;
	};

//----------------------------------------------------------------------

#endif