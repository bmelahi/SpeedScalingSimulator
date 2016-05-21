
#ifndef _DESLogger_h_

	#define _DESLogger_h_

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
	#include <algorithm>

	#include "yLog.h"	
	#include "Job.h"
	#include "PowerFunction.h"
	#include "GlobalsAndTypes.h"

//----------------------------------------------------------------------

	class DESLogger {
	public:
		DESLogger() : avgT_m(0), avgE_m(0), totalN_m(0), firstNjobsLeft_m(0), jobCount_m(0) {}
		virtual ~DESLogger() = 0 {}

	public:
		virtual std::string toString() = 0;		// Returns the name and parameters of the object

		virtual void arrival_handler(ArrivalEvent *) = 0;						// Handles an arrival event
		virtual void departure_handler(DepartureEvent *, Job *) = 0;			// Handles a departure event, must free the job
		virtual void speedchange_handler(double time, double newspeed) = 0;				// Handles a speed-change event
		
		virtual void execution_handler(std::vector<unsigned long> & jobs,		// Handles recording of the executions 
									   std::vector<double> & ratePerJob, 
									   double time1, 
									   double time2
									   ) = 0;
		
		// Returns the power consumed
		double powerConsumed(double speed, double interval, PowerFunction * P)	{
			return P->function(speed) * interval;
		}
		
		unsigned long getTotalN() {return totalN_m;}
		unsigned long getfirstNjobsLeft() { return firstNjobsLeft_m; }
		void decreaseFirstNjobsLeft() { firstNjobsLeft_m--; }

		// Discards a job, without  considering it for the reports, must delete the job pointer
		void discardJob(Job * j) {
			if (jobCount_m <= 0)
				yLog::logtime(ERRORLOG, __FUNCTION__, "Discarding a job while jobCount <= 0!");
			else
				jobCount_m--;
			if (j != NULL)
				delete j;
		}

		// Writes the job report on the given log file, must delete the job
		void writeJobReport (Job * job, Logfile_Type logfile, unsigned long id) {
			//yLog::log(REPORTLOG, "JobID \t size \t Arrival \t Departure \t Energy \t ExecutionT \t  ResponseT \t   Slowdown \t AvgSpeed");
			yLog::log(logfile, "%6d \t %10.10f \t %10.10f \t %10.10f \t %10.10f \t %10.10f \t %10.10f \t %10.10f \t %10.10f", 
				id,
				job->getSize(),
				job->getArrival(),
				job->departure_m,
				job->energyConsumed_m,
				job->executiontime_m,
				job->departure_m - job->getArrival(), 
				(job->departure_m - job->getArrival())/job->getSize(),
				(job->getSize()/job->executiontime_m)
				);
			delete job;
		}

		// Sort completedJobes_m by size, write the job reports. Clear completedJobes_m
		void clear (Logfile_Type logfile) {
			std::sort(completedJobs_m.begin(), completedJobs_m.end(), jobPointerSort);
			while (completedJobs_m.size() > 0) {
				Job * job = completedJobs_m.back();
				completedJobs_m.pop_back();
				unsigned long id = (job->isProbe())? job->getProbeID() : job->getID();
				writeJobReport(job, logfile, id);
			}
			//yLog::log(MEANZLOG, "      E[T] \t       E[e] \t           Z \t           N");
			if (totalN_m != 0)
				yLog::log(MEANZLOG, "%10f \t %10f \t %10f \t %10d", avgT_m / (double)totalN_m, avgE_m / (double)totalN_m, (avgT_m + avgE_m) / (double)totalN_m, totalN_m);
			avgT_m = avgE_m = 0;
			totalN_m = 0;
			firstNjobsLeft_m = 0;
			jobCount_m = 0;
		}


	protected:
		double avgT_m;
		double avgE_m;
		unsigned long totalN_m;
		unsigned long firstNjobsLeft_m;
		unsigned long jobCount_m;

		vector<Job *> completedJobs_m;
		bool inDebugMode_m;
		bool inPromptMode_m;
	};

//----------------------------------------------------------------------

#endif