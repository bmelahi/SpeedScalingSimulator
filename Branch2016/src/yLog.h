
#ifndef __Y_LOG_H

	#define __Y_LOG_H

//----------------------------------------------------------------------

	#include <string>
	
	using namespace std;
	
//----------------------------------------------------------------------

	class yLog
	{
	public :
		enum {MaxLogChannels = 11, LogBufferSize = 14096};	// Totally arbitrary!

	public :
		static bool setLogFile (const char * filename, int channel = 0);
		static string getLogFile (int channel = 0);
		static bool clearLogFile (int channel);
		static int setUTCOffset (int minutes);	// A value of -24 * 60 causes localtime() to be used instead of gmtime()
		static bool flushall();

		static int log (int channel, const char * fmt, ...);
		static int logtime (int channel, const char * functionname, const char * fmt, ...);
		
	private :
		yLog () {}
		yLog (const yLog &) {}
		~yLog () {}
		yLog & operator = (const yLog &) {return *this;}
		
	private :
		static string logfilename [MaxLogChannels];
		static FILE * logfilepointer [MaxLogChannels];
		static int utcoffset;
	};
	
//----------------------------------------------------------------------
	
#endif	// __Y_LOG_H
