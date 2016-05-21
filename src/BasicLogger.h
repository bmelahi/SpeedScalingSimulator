
#ifndef _BasicLogger_h_

	#define _BasicLogger_h_

/*----------------------------------------------------------------------
* DESLogger.h - definitions/declarations for class DESLogger
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
* Purpose:
*       This file defines the class DESLogger, with its members and methods.
*
*///---------------------------------------------------------------------
	
	#include <string>
	#include <vector>

	#include "Job.h"
	#include "Event.h"
	#include "DESLogger.h"
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class BasicLogger: public DESLogger {
	public:
		BasicLogger(std::string debugmode, std::string writemode, std::string path, std::string simName);
		~BasicLogger();

	public:
		std::string toString();		// Returns the name and parameters of the object

		void arrival_handler(ArrivalEvent *);						// Handles an arrival event
		void departure_handler(DepartureEvent *, Job *);			// Handles a departure event, must free the job
		void speedchange_handler(double time, double newspeed);				// Handles a speed-change event
		
		void execution_handler(std::vector<unsigned long> & jobs,	// Handles recording of the executions 
							   std::vector<double> & ratePerJob, 
									   double time1, 
									   double time2
									   );
	private:
		void setSimulationFolderNameByTime(std::string path, wchar_t * outputFolder, string simName);

	private:
		static std::string name_s;
	};

//----------------------------------------------------------------------

#endif