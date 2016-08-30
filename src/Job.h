
#ifndef _Job_h_

	#define _Job_h_

/*----------------------------------------------------------------------
*job.h - definitions/declarations for class Job
*
*       Written by Maryam Elahi
*		Last updated: Nob, 2012
*
*Purpose:
*       This file defines the class of Job, with its members and methods.
*
*///--------------------------------------------------------------------

	#include "GlobalsAndTypes.h"
	
	using namespace std;
	
//----------------------------------------------------------------------

	class Job {

//----------------------------------------------------------------------
// Constructors:
//----------------------------------------------------------------------
	public:
		Job () : 
		  size_m(0), 
		  estimatedsize_m(0),
		  remsize_m(0),
		  estimatedremsize_m(0),
		  id_m (0), 
		  arrival_m(0), 
		  deadline_m(0), 
		  departure_m(0), 
		  isProbe_m(false),
		  isFinished_m (false), 
		  isBeingExecuted_m(false),
		  responsetime_m(0),
		  executiontime_m(0),
		  energyConsumed_m(0)
		  {}
		  
  		Job (double arrival, unsigned long id, double size) : 
		  size_m(size), 
		  estimatedsize_m(size),
		  remsize_m(size),
		  estimatedremsize_m(size),
		  id_m (id), 
		  arrival_m(arrival), 
		  deadline_m(0), 
		  departure_m(0),
		  isProbe_m(false),
		  isFinished_m (false), 
		  isBeingExecuted_m(false), 
		  responsetime_m(0),
		  executiontime_m(0),
		  energyConsumed_m(0)
		  {}


		Job (double arrival, unsigned long id, double size, double deadline) : 
		  size_m(size), 
		  estimatedsize_m(size),
		  remsize_m(size),
		  estimatedremsize_m(size),
		  id_m (id), 
		  arrival_m(arrival), 
		  deadline_m(deadline), 
		  departure_m(0),
		  isProbe_m(false),
		  isFinished_m (false), 
		  isBeingExecuted_m(false),
		  responsetime_m(0),
		  executiontime_m(0),
		  energyConsumed_m(0)
		  {}

		Job(double arrival, unsigned long id, double size, double deadline, double estimated) :
			size_m(size),
			estimatedsize_m(estimated),
			remsize_m(size),
			estimatedremsize_m(estimated),
			id_m(id),
			arrival_m(arrival),
			deadline_m(deadline),
			departure_m(0),
			isProbe_m(false),
			isFinished_m(false),
			isBeingExecuted_m(false),
			responsetime_m(0),
			executiontime_m(0),
			energyConsumed_m(0)
		{}

//----------------------------------------------------------------------
// Methods and Operators:
//----------------------------------------------------------------------

	public:
		bool operator<(const Job & j) const;

		void set_probe(unsigned long probe_id);

		double getSize() const {return size_m;}
		double getEstimatedSize() const {return estimatedsize_m;}
		unsigned long getID() const {return id_m;}
		unsigned long getProbeID() const {return probeid_m;}
		double getArrival() const {return arrival_m;}
		double getDeadline() const {return deadline_m;}
		bool isProbe() const {return isProbe_m;}

//----------------------------------------------------------------------
	// Member variables:
//----------------------------------------------------------------------
	private:
		double size_m;					// Job size (real size)
		double estimatedsize_m;			// Job size estimation
		unsigned long id_m;				// Job id
		double arrival_m;				// The arrival time
		double deadline_m;				// The deadline (0 if not valid)
		bool isProbe_m;				

	public:
		double remsize_m;				// The remaining work of the job (of the real size)
		double estimatedremsize_m;		// The remaining work based on the job size estimation
		double departure_m;				// The actual departure time
	
		bool isFinished_m;				// ?
		bool isBeingExecuted_m;			// The current executing job

		unsigned long probeid_m;		// If the job is a probe, the probe id
		double responsetime_m;			// Job's response time
		double executiontime_m;			// The actual time that job received service
		double energyConsumed_m;		// Job's energy consumption
	};

//----------------------------------------------------------------------

	bool jobPointerSort(const Job * j1, const Job * j2);

//----------------------------------------------------------------------


#endif