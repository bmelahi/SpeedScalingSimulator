
#ifndef _GlobalsAndTypes_h_

	#define _GlobalsAndTypes_h_

/*----------------------------------------------------------------------
* GlobalsAndTypes.h - Global definitions/declarations 
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 20, 2012
*
* Purpose:
*       Global Definitions and Variables
*
*///--------------------------------------------------------------------

	#include<limits>

	using namespace std;

//----------------------------------------------------------------------

	/* Type Definitions */
	typedef enum Event_Type {
					INVALID = 0,			// Invalid event, used for initialization and invalidation of events 
				  SCHEDULER = 1,			// Scheduler event, used for time driven simulation with a quantum
		 DEPARTURE_EXPECTED = 2,			// Expected departure event, indicates the forcasted departure, may be invalidated later
		   DEPARTURE_ACTUAL = 3,			// Actual departure event, indicates when a job really finishes
					  ABORT = 4,			// Abort event, indicates when a job gives up
			   SPEED_CHANGE = 5,			// Speed change event, for decoupled speed scaling, indicates a speed change
					ARRIVAL = 6,			// Arrival event, indicates the arrival of a new job
	};

//----------------------------------------------------------------------

	typedef enum Logfile_Type {
					PROGRESSLOG = 0,			// Records the progression of the DES
					   ERRORLOG = 1,			// Records the error log
					   DEBUGLOG = 2,			// Detailed debug output
					SIMPARAMLOG = 3,			// The simulation parameters
					WLREPORTLOG = 4,			// The simulation summary for the workload without the probe
				 PROBEREPORTLOG = 5,			// The simulation summary for the probes
					    BYTELOG = 6,			// The byte profile for the workload without the probe
					   SPEEDLOG = 7,			// The speed profile for the workload without the probe
					JOBCOUNTLOG = 8,			// The job count profile for the workload without the probe
					   MEANZLOG = 9,			// The mean responsetime and energy consumption, per each round
					  QUEUEDUMP = 10,			// The queue status at arrivals and departures
	};

//----------------------------------------------------------------------

	typedef enum JobID_Type {
					  INVALIDID = 0,			// Invalid job ID 
					PROBEIDBASE = 1,			// Base probe ID
					  JOBIDBASE = 2,			// First valid job ID
	};

//----------------------------------------------------------------------

	/* Constants */
	//const double DOUBLE_PRECISION = 0.000001;
	const double DOUBLE_PRECISION_EPSILON = std::numeric_limits<double>::epsilon() * 10;///2.0; 
	const double LOOSE_DOUBLE_PRECISION_EPSILON = 0.0000001;  
		
//----------------------------------------------------------------------

	/* Constant functions */
	
	/* Functions for handling comparison of type double. 
	   The Knuth algorithm for close enough with tolerance DOUBLE_PRECISION is used.
	*/
	const bool approximatelyEqual(const double a, const double b);

	const bool definitelyGreaterThan(const double a, const double b);

	const bool definitelyLessThan(const double a, const double b);

//----------------------------------------------------------------------
//----------------------------------------------------------------------

#endif

