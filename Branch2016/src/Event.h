
#ifndef _Event_h_

	#define _Event_h_

/*----------------------------------------------------------------------
* Event.h - definitions/declarations for class Event
*
*       Written by Maryam Elahi
*		Last (remembered) updated: Nov 2012
*
*Purpose:
*       This file defines the class Event, with its members and methods.
*
*///---------------------------------------------------------------------

	#include "Job.h"
	#include "yLog.h"
	#include "GlobalsAndTypes.h"

class Event {
	public:
		Event(): time(0), type (INVALID), job_id(0), validId_m(0) {}
		Event(double t, Event_Type typ, int id, unsigned long validid) : time(t), type(typ), job_id(id), validId_m(validid) {}

	public:
		class CompareEvents {
			public:
			bool operator()(const Event * e1, const Event * e2) const // Returns false if e1 should run earlier than e2 
			{
				if (approximatelyEqual(e1->time, e2->time)) {
					if (e1->type == e2->type) {
						if (e1->validId_m == e2->validId_m) {
							yLog::logtime(ERRORLOG, __FUNCTION__, "Found equal ValidIDs: %d of the same type: %d !", 
																								e1->validId_m, e1->type);
							return e1 > e2;
						}
						else
							return e1->validId_m > e2->validId_m;
					}
					else
						return (e1->type > e2->type);
				}
				else if (definitelyLessThan(e1->time, e2->time))
					return false;
				else 
					return true;
			}
		};

	public:
		double time;
		Event_Type type;
		unsigned long job_id;
		unsigned long validId_m;	// For the invalidation mechanism
	};

//----------------------------------------------------------------------

	class ArrivalEvent: public Event {
	public:
		ArrivalEvent(double t, Event_Type typ, int id, Job job, unsigned long validid) 
			: Event(t, typ, id, validid), job_m(job) {}

	public:
		Job job_m;
	};

//----------------------------------------------------------------------

	class DepartureEvent: public Event {
	public:
		DepartureEvent(double t, Event_Type typ, int id, unsigned long validid) 
			: Event(t, typ, id, validid) {}
	};

//----------------------------------------------------------------------

	class SpeedChangeEvent: public Event {
	public:
		SpeedChangeEvent(double t, Event_Type typ, int id, double speed, unsigned long validid) 
			: Event(t, typ, id, validid), newSpeed_m(speed) {}

	public:
		double newSpeed_m;
	};

//----------------------------------------------------------------------

	class SchedulerEvent: public Event {
	public:
		SchedulerEvent(double t, Event_Type typ, int id, unsigned long validid, Event_Type childtype) 
			: Event(t, typ, id, validid) , child_type (childtype) {}

	public:
		Event_Type child_type;
	};

//----------------------------------------------------------------------

#endif