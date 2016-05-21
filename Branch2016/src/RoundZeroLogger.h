
#ifndef _RoundZeroLogger_h_

	#define _RoundZeroLogger_h_

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

	class RoundZeroLogger: public DESLogger {
	public:
		RoundZeroLogger(std::string debugmode, std::string writemode);
		virtual ~RoundZeroLogger();

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
		static std::string name_s;

	private:
		double byteCount_m;
		double prevSpeed_m;
		double prevTime_m;
		double prevExecutionTime_m;
		bool zeroRemaining_m;
	};

//----------------------------------------------------------------------

#endif