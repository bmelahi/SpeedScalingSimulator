
#pragma warning( disable : 4996 )

#include "yLog.h"

#include <ctime>
#include <cstdio>
#include <cstdarg>

//----------------------------------------------------------------------
// yLog class implementation :
//----------------------------------------------------------------------

string yLog::logfilename [yLog::MaxLogChannels] = {"log.txt"};
FILE * yLog::logfilepointer [yLog::MaxLogChannels] = {NULL};
int yLog::utcoffset = 0;	// Default to GMT

//----------------------------------------------------------------------

bool yLog::setLogFile (const char * filename, int channel)
{
	if (channel < 0 || channel >= MaxLogChannels)
		return false;

	FILE * of = fopen (filename, "at");	// Check for file write permissions. This may not be a good idea.
	if (of == NULL)
		return false;
		
	//fclose (of);
	logfilename[channel] = filename;
	logfilepointer[channel] = of;
	return true;
}

//----------------------------------------------------------------------

string yLog::getLogFile (int channel)
{
	if (channel < 0 || channel >= MaxLogChannels)
		return "";
	else
		return logfilename[channel];
}

//----------------------------------------------------------------------

bool yLog::clearLogFile (int channel)
{
	if (channel < 0 || channel >= MaxLogChannels)
		return false;
		
	FILE * of = fopen (logfilename[channel].c_str(), "wt");
	if (of == NULL) 
		return false;
	fclose (of);
	return true;
}

//----------------------------------------------------------------------

bool yLog::flushall ()
{
	for (int i = 0; i < MaxLogChannels; i++) {
		if( logfilepointer[i] == NULL)
			continue;
		fflush(logfilepointer[i]);
	}
	return true;
}

//----------------------------------------------------------------------

int yLog::setUTCOffset (int minutes)
{
	int r = utcoffset;
	
	utcoffset = minutes;
	
	if (utcoffset == -24 * 60) return r;
	
	// Normalize utcoffset to GMT +- 12 hours.
	while (utcoffset > 12 * 60) utcoffset -= 24 * 60;
	while (utcoffset < -12 * 60) utcoffset += 24 * 60; 
	
	return r;
}

//----------------------------------------------------------------------

int yLog::logtime (int channel, const char * functionname, const char * fmt, ...)
{
	static char buffer [LogBufferSize];
	 
	if (channel < 0 || channel >= MaxLogChannels)
		return -1;
			
	if (logfilename[channel] == "")
		return -1;

	//FILE * of = fopen (logfilename[channel].c_str(), "at");
	//if (of == NULL) return -1;
	FILE * of = logfilepointer[channel];
	
	va_list args;
	
	va_start (args, fmt);
	int r = vsprintf (buffer, fmt, args);
	va_end (args);
	if (r < 0) return r;
	
	time_t ut;
	time (&ut);
	tm * ts = NULL;

	if (utcoffset == -24 * 60)
		ts = localtime (&ut);
	else
	{
		ut += utcoffset * 60;
		ts = gmtime (&ut);
	}
	
	//r = fprintf (of, "%04d-%02d-%02d %02d:%02d:%02d --> %s\n", 
	//		1900 + ts->tm_year, ts->tm_mon + 1, ts->tm_mday,
	//		ts->tm_hour, ts->tm_min, ts->tm_sec, buffer);

	r = fprintf (of, "%02d:%02d:%02d (in %s) --> %s\n", 
			ts->tm_hour, ts->tm_min, ts->tm_sec, functionname, buffer);

	//fclose (of);		
	return r;
}

//----------------------------------------------------------------------

int yLog::log (int channel, const char * fmt, ...)
{
	static char buffer [LogBufferSize];
	
	if (channel < 0 || channel >= MaxLogChannels)
		return -1;
	
	if (logfilename[channel] == "")
		return -1;
		
	//FILE * of = fopen (logfilename[channel].c_str(), "at");
	//if (of == NULL) return -1;
	FILE * of = logfilepointer[channel];
	
	va_list args;
	
	va_start (args, fmt);
	int r = vsprintf (buffer, fmt, args);
	va_end (args);
	if (r < 0) return r;
	
	time_t ut;
	time (&ut);
	tm * ts = NULL;

	if (utcoffset == -24 * 60)
		ts = localtime (&ut);
	else
	{
		ut += utcoffset * 60;
		ts = gmtime (&ut);
	}
	
	r = fprintf (of, "%s\n", buffer);

	//fclose (of);		
	return r;
}
