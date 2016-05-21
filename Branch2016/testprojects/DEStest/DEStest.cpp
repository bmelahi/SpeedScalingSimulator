/***
* DEStest.cpp - The tester for DES
*
*       Written by Maryam Elahi
*		Last updated: Nov, 2012
*
* Purpose:
*       This is the main file of the solution.
*
****/

#pragma warning( disable : 4482 )
#pragma warning( disable : 4996 )

#include <Windows.h>

#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <queue>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "GlobalsAndTypes.h"
#include "yLog.h"

//#include "Job.h"
//#include "Event.h"

#include "DES.h"
#include "SRPT.h"
#include "SingleSpeed.h"
#include "BasicLogger.h"
#include "ProbeSetting.h"
#include "Configuration.h"
#include "WorkloadFileWrapper.h"

using namespace std;

//----------------------------------------------------------------------

const string conf_file = "Config.txt";

bool createProbeAvgFile();
bool createBusyPeriodReport(Configuration *);
void fixbyteprofile();

//----------------------------------------------------------------------

int main (int argc, int* argv) {

	// Start of a new simulation: say hi!
	cout << "=========================================================" << endl
		 << "Welcom to S3: The Speed Scaling Simulation!" << endl
		 << "=========================================================" << endl
		 << endl;

	// Read configuration file, setup Logger
	Configuration * config = new Configuration;
	DESLogger * logger = config->configurationReader(string(conf_file));
	if (logger == NULL) {
		cerr << "Could not build the logger, exiting the simulation!" << endl;
		return 0;
	}
	else
		cout << endl << "Handing over to the logger." << endl;

	// Write the simulation parameters
	yLog::log(Logfile_Type::SIMPARAMLOG, "%% Simulation configuration:\n%s", config->printSimParam().c_str());

	// Setup the probesimulator
	ProbeSetting probesim(logger, config);

	// Run the probesimulator
	if (probesim.run()) {
		yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "Done!");
		cout << "Finished S3. Enjoy hours of post porcessing!" << endl;
		yLog::flushall();
		createProbeAvgFile();
		//fixbyteprofile();
		//createBusyPeriodReport(config);
		
	}
	else {
		yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "Failed!");
		cout << "Failed S3. Enjoy hours of debugging!" << endl;
	}

	//// Run the probesimulator
	//if (probesim.runLoad()) {
	//	yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "Done!");
	//	cout << "Finished S3. Enjoy hours of post porcessing!" << endl;
	//	yLog::flushall();
	//	createProbeAvgFile();
	//}
	//else {
	//	yLog::logtime(Logfile_Type::PROGRESSLOG, __FUNCTION__, "Failed!");
	//	cout << "Failed S3. Enjoy hours of debugging!" << endl;
	//}


	// Clean up
	delete logger;
	delete config;
	return 0;
}

//----------------------------------------------------------------------


bool createProbeAvgFile() {
	ifstream fin (yLog::getLogFile(PROBEREPORTLOG));
	FILE * of = fopen ("probeaverages.txt", "wt");
	fprintf(of, " probe-Size \t avg-Energy \t avg-ExecTime \t avg-RespTime \t avg-Slowdown\n");

	string line;
	getline(fin, line);
	getline(fin, line);
	double avgsize = 0, avgeng = 0, avgexec = 0, avgT = 0, avgsl = 0;
	int cnt = 0;
	
	while (line != "") {
		// Format:  JobID 	       size 	    Arrival 	  Departure 	     Energy 	 ExecutionT 	  ResponseT 	   Slowdown
		int id;
		stringstream ss;
		ss << line;
		double size, arrival, departure, energy, execT, respT, slowdown;
		ss >> id >> size >> arrival >> departure >> energy >> execT >> respT >> slowdown;
		if ((avgsize == 0) || size == avgsize) {
			cnt++;
			avgsize = size;
			avgeng += energy;
			avgexec += execT;
			avgT += respT;
			avgsl += slowdown;
		}
		else {
			fprintf(of, "%10f\t%10f\t%10f\t%10f\t%10f\n", avgsize, avgeng/cnt, avgexec/cnt, avgT/cnt, avgsl/cnt);
			cnt = 1;
			avgsize = size;
			avgeng = energy;
			avgexec = execT;
			avgT = respT;
			avgsl = slowdown;
		}
		getline(fin, line);
	}
	fprintf(of, "%10f\t%10f\t%10f\t%10f\t%10f\n", avgsize, avgeng/cnt, avgexec/cnt, avgT/cnt, avgsl/cnt);
	fclose(of);
	return true;
}


//----------------------------------------------------------------------
// Fix workloadbyteprofile

void fixbyteprofile() {
	ifstream originalbytes(yLog::getLogFile(BYTELOG));
	string filename = "fixed" + yLog::getLogFile(BYTELOG);
	FILE * of = fopen(filename.c_str(), "wt");
	//ofstream fixedbytes("fixed" + yLog::getLogFile(BYTELOG));

	string firstline;
	getline(originalbytes, firstline);
	//fixedbytes << firstline << endl;
	fprintf(of, "%s\n", firstline.c_str());
	
	double time, value;
	double lasttime, lastvalue;
	originalbytes >> time >> value;
	lasttime = time;
	lastvalue = value;	

	while (!originalbytes.eof()) {
		if (approximatelyEqual(lastvalue, 0.0))	{
			fprintf(of, "%10f \t %10f\n", time, 0.0);
			//fixedbytes << time << "\t" << 0.0 << endl;
		}
		lasttime = time;
		lastvalue = value;

		fprintf(of, "%10f \t %10f\n", time, value);
		//fixedbytes << time << "\t" << value << endl;
		originalbytes >> time >> value;
	}

	fclose(of);
	originalbytes.close();
}

//----------------------------------------------------------------------



class BusyPeriod {
public:
	BusyPeriod() :
		index(0), 
		starttime(0),
		endtime(0),
		duration(0),

		totaljobcnt(0),
		maxjobcnt(0), 
		occupancyintegral(0),
		avgoccupancy(0),

		totalbytecnt(0),
		maxbytecnt(0),
		byteintegral(0),
		avgragebyte(0),

		totalenergy(0),
		maxspeed(0),
		speedintegral(0),
		avgspeed(0),

		avgT(0),
		avgSL(0)
	{}

	BusyPeriod(
		int INX, 
		double StartTime,
		double EndTime,
		double Duration,

		int TotalJobCnt,
		int Maxjobcnt, 
		double Occupancyintegral,
		double Avgoccupancy,

		double Totalbytecnt,
		double Maxbytecnt,
		double Byteintegral,
		double Avgragebyte,

		double Totalenergy,
		double Maxspeed,
		double Speedintegral,
		double Avgspeed,

		double AvgT,
		double AvgSL
		) :
			index(INX),
			starttime(StartTime),
			endtime(EndTime),
			duration(Duration),

			totaljobcnt(TotalJobCnt),
			maxjobcnt(Maxjobcnt),
			occupancyintegral(Occupancyintegral),
			avgoccupancy(Avgoccupancy),

			totalbytecnt(Totalbytecnt),
			maxbytecnt(Maxbytecnt),
			byteintegral(Byteintegral),
			avgragebyte(Avgragebyte),

			totalenergy(Totalenergy),
			maxspeed(Maxspeed),
			speedintegral(Speedintegral),
			avgspeed(Avgspeed),

			avgT(AvgT),
			avgSL(AvgSL)
	{}

	void clear()
	{
		index = 0;
		starttime = 0;
		endtime = 0;
		duration = 0;

		totaljobcnt = 0;
		maxjobcnt = 0;
		occupancyintegral = 0;
		avgoccupancy = 0;

		totalbytecnt = 0;
		maxbytecnt = 0;
		byteintegral = 0;
		avgragebyte = 0;

		totalenergy = 0;
		maxspeed = 0;
		speedintegral = 0;
		avgspeed = 0;

		avgT = 0;
		avgSL = 0;
	}

	bool print(FILE * file) {
		fprintf(file, "\
%10d\t\
%10f\t\
%10f\t\
%10f\t\
\
%10d\t\
%10d\t\
\
%10f\t\
%10f\t\
\
%10f\t\
%10f\t\
%10f\t\
%10f\t\
\
%10f\t\
%10f\t\
%10f\t\
%10f\t\
\
%10f\t\
%10f\t\
\n",
			index, 
			starttime,
			endtime,
			duration,
			totaljobcnt,
			maxjobcnt,
			occupancyintegral,
			avgoccupancy,
			totalbytecnt,
			maxbytecnt,
			byteintegral,
			avgragebyte,
			totalenergy,
			maxspeed,
			speedintegral,
			avgspeed,
			avgT,
			avgSL
		);		
		return true; // TODO: set return value based on successful write.
	}

public:
	int index;

	double starttime;
	double endtime;
	double duration;

	int totaljobcnt;
	int maxjobcnt;
	double occupancyintegral;
	double avgoccupancy;

	double totalbytecnt;
	double maxbytecnt;
	double byteintegral;
	double avgragebyte;

	double totalenergy;
	double maxspeed;
	double speedintegral;
	double avgspeed;

	double avgT;
	double avgSL;
};

bool computeBytes(BusyPeriod &currentBP);
bool computeSpeeds(BusyPeriod &currentBP, Configuration * cf);
bool computeExp(BusyPeriod &currentBP);

bool createBusyPeriodReport(Configuration * cf) {

	ifstream fin(yLog::getLogFile(JOBCOUNTLOG));
	FILE * of = fopen("busyperiodreport.txt", "wt");
	//fprintf(of, "bp-index \t start-time \t end-time \t duration \t total-job-cnt\tmax\tocc-intl\tocc-avg\t\n");
	fprintf(of, "%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t%10s\t\n",
		"index",
		"starttime",
		"endtime",
		"duration",
		"totaljobs",
		"maxjobcnt",
		"occpncyint",
		"avgoccpncy",
		"totalbytes",
		"maxbytecnt",
		"byteint",
		"avgbyte",
		"totaleng",
		"maxspeed",
		"speedint",
		"avgspeed",
		"avgT",
		"avgSL"
		);
	for (unsigned dash = 0; dash < 215; dash++)
		fprintf(of, "-");
	fprintf(of, "\n");

	double timestamp, prevtimestamp = 0;
	int jobcnt = 0, lastjobcnt = 0, bpcount = 0;
	string dummy;
	bool idle = false;

	vector <BusyPeriod> recordBusyPeriods;
	BusyPeriod currentBP;
	
	fin >> dummy >> dummy >> dummy >> dummy >> dummy;
	fin >> timestamp >> jobcnt;

	currentBP.totaljobcnt = 0;
	currentBP.starttime = timestamp;

	//cout << timestamp << "\t" << jobcnt << endl;
	bool lastbptakencareof = false;
	
	while (!fin.eof() || !lastbptakencareof) {
		if ((idle == true && jobcnt != 0) || fin.eof()) {
			
			currentBP.endtime = prevtimestamp;
			currentBP.avgoccupancy = currentBP.occupancyintegral / currentBP.duration;

			if (!computeBytes(currentBP))
				cout << "ERROR! problem with computing byte reports" << endl;
			if (!computeSpeeds(currentBP, cf))
				cout << "ERROR! problem with computing speed reports" << endl;
			if (!computeExp(currentBP))
				cout << "ERROR! problem with computing expectations reports" << endl;
			recordBusyPeriods.push_back(BusyPeriod(currentBP));
			lastbptakencareof = true;

			currentBP.clear();
			currentBP.index = ++bpcount;
			idle = false;
			lastjobcnt = jobcnt;
			prevtimestamp = timestamp;

			currentBP.totaljobcnt = 1;
			currentBP.starttime = timestamp;

			continue;
		}

		if (jobcnt > lastjobcnt) {
			currentBP.totaljobcnt += 1;
		}
		currentBP.occupancyintegral += lastjobcnt * (timestamp - prevtimestamp);
		currentBP.duration += (timestamp - prevtimestamp);

		prevtimestamp = timestamp;
		lastjobcnt = jobcnt;

		if (jobcnt > currentBP.maxjobcnt)
			currentBP.maxjobcnt = jobcnt;

		if (jobcnt == 0)
			idle = true;
		else
			lastbptakencareof = false;

		fin >> timestamp >> jobcnt;
	}

	cout << "Printing out the busyperiods.txt file." << endl;
	double avgofavgoccupancy = 0;
	double avgoverallspeed = 0;

	for (unsigned i = 0; i < recordBusyPeriods.size(); i++) {
		recordBusyPeriods[i].print(of);

		if (recordBusyPeriods[i].avgoccupancy == 0)
			cout << "Error, bp with index " << recordBusyPeriods[i].index << " has zero avg-occupancy!" << endl;
		avgofavgoccupancy += recordBusyPeriods[i].avgoccupancy;
		avgoverallspeed += recordBusyPeriods[i].speedintegral;
	}

	double busyportion = 0;
	double util = 0;
	for (unsigned i = 0; i < recordBusyPeriods.size(); i++) {
		busyportion += recordBusyPeriods[i].duration;
	}
	util = busyportion / recordBusyPeriods[recordBusyPeriods.size() - 1].endtime;
	avgoverallspeed /= busyportion;

	cout << "Utilization: " << util << endl;

	avgofavgoccupancy = avgofavgoccupancy / bpcount;
	cout << "Number of busy periods: " << bpcount << endl;
	cout << "Average observed occupancy: " << avgofavgoccupancy << endl;
	cout << "Average speed: " << avgoverallspeed << endl;

	FILE * ofavg = fopen("busyperiodaverages.txt", "wt");
	fprintf(ofavg, "%13s\t%13s\t%13s\t%13s\n",
		"BusyPeriods",
		"Utilization",
		"AvgSpeed",
		"AvgOccupancy");
	fprintf(ofavg, "%13d\t%13f\t%13f\t%13f\n", bpcount, util, avgoverallspeed, avgofavgoccupancy);

	return true;
}

//----------------------------------------------------------------------

// Compute :
			// totalbytecnt,
			// maxbytecnt,
			// byteintegral,
			// avgragebyte,

bool computeBytes(BusyPeriod &currentBP)
{
	if (approximatelyEqual(currentBP.endtime, currentBP.starttime))
		return false;

	ifstream finbytes("fixed" + yLog::getLogFile(BYTELOG));
	string dummy;

	double time, bytes, lasttime, lastbytes;
	currentBP.totalbytecnt = 0;
	currentBP.maxbytecnt = 0;
	currentBP.byteintegral = 0;
	currentBP.avgragebyte = 0;

	getline(finbytes, dummy);
	finbytes >> time >> bytes;
	
	while (!approximatelyEqual(time, currentBP.starttime))
		finbytes >> time >> bytes;

	lasttime = time; lastbytes = bytes;
	
	while (!approximatelyEqual(time, currentBP.endtime) && !finbytes.eof()){
		if (definitelyGreaterThan(bytes, lastbytes))
			currentBP.totalbytecnt += (bytes - lastbytes);
		if (!approximatelyEqual(time, lasttime) && !approximatelyEqual(lastbytes, 0.0))
		{
			currentBP.byteintegral += (lastbytes * (time - lasttime)) - ((lastbytes - bytes) * (time - lasttime) / 2);
			if (definitelyGreaterThan(bytes, lastbytes))
				cout << "Unexpected Reverse jump in byte count! ********************************" << endl;
		}
		
		if (bytes > currentBP.maxbytecnt)
			currentBP.maxbytecnt = bytes;

		lasttime = time; lastbytes = bytes;
		finbytes >> time >> bytes;
	}
	if (!approximatelyEqual(time, lasttime) && !approximatelyEqual(lastbytes, 0.0))
	{
		currentBP.byteintegral += (lastbytes * (time - lasttime)) - ((lastbytes - bytes) * (time - lasttime) / 2);
		if (definitelyGreaterThan(bytes, lastbytes))
			cout << "Unexpected Reverse jump in byte count! ********************************" << endl;
	}
	currentBP.avgragebyte = currentBP.byteintegral / (currentBP.endtime - currentBP.starttime);
	
	return true;
}

//----------------------------------------------------------------------


// Compute :
			// totalenergy,
			// maxspeed,
			// speedintegral,
			// avgspeed,
bool computeSpeeds(BusyPeriod &currentBP, Configuration * cf)
{
	if (approximatelyEqual(currentBP.endtime, currentBP.starttime))
		return false;

	ifstream finspeed(yLog::getLogFile(SPEEDLOG));

	string dummy;
	double time, speed, lasttime, lastspeed;
	currentBP.totalenergy = 0;
	currentBP.maxspeed = 0;
	currentBP.speedintegral = 0;
	currentBP.avgspeed = 0;

	getline(finspeed, dummy);
	finspeed >> time >> speed;

	while (!approximatelyEqual(time, currentBP.starttime))
		finspeed >> time >> speed;

	lasttime = time; lastspeed = speed;

	while (!approximatelyEqual(time, currentBP.endtime) && !finspeed.eof()){
		// Computing speedintegral
		// Computing totalenergy
		if (!approximatelyEqual(time, lasttime) && !approximatelyEqual(lastspeed, 0.0))
		{
			currentBP.speedintegral += (lastspeed * (time - lasttime));
			currentBP.totalenergy += (cf->PowerFunctionFactory(cf->getPowerfunctionConfig())->function((lastspeed))* (time - lasttime));
		}

		// Computing maxspeed
		if (speed > currentBP.maxspeed)
			currentBP.maxspeed = speed;

		lasttime = time; lastspeed = speed;
		finspeed >> time >> speed;
	}
	if (!approximatelyEqual(time, lasttime) && !approximatelyEqual(lastspeed, 0.0))
	{
		currentBP.speedintegral += (lastspeed * (time - lasttime));
		currentBP.totalenergy += (cf->PowerFunctionFactory(cf->getPowerfunctionConfig())->function((lastspeed))* (time - lasttime));
	}
	currentBP.avgspeed = currentBP.speedintegral / (currentBP.endtime - currentBP.starttime);

	return true;
}

//----------------------------------------------------------------------


bool computeExp(BusyPeriod &currentBP)
{
	if (approximatelyEqual(currentBP.endtime, currentBP.starttime))
		return false;

	ifstream finwlreport(yLog::getLogFile(WLREPORTLOG));

	string dummy;
	int inx;
	double size, arrival, departure, energy, execution, T, SL;
	currentBP.avgT = 0;
	currentBP.avgSL= 0;
	int cnt = 0;

	getline(finwlreport, dummy);
	finwlreport >> inx >> size >> arrival >> departure >> energy >> execution >> T >> SL;
	
	while (!finwlreport.eof()){
		if ((definitelyGreaterThan(arrival, currentBP.starttime) || approximatelyEqual(arrival, currentBP.starttime))
			&& (definitelyGreaterThan(currentBP.endtime, arrival) || approximatelyEqual(currentBP.endtime, arrival)))
		{
			cnt++;
			currentBP.avgT += T;
			currentBP.avgSL += SL;
		}
		finwlreport >> inx >> size >> arrival >> departure >> energy >> execution >> T >> SL;
	}

	if (cnt == 0) {
		return false;
	}

	currentBP.avgT /= cnt;
	currentBP.avgSL /= cnt;

	if (cnt != currentBP.totaljobcnt)
		cout << "Job-count computed for busy period " << currentBP.index << " is " << currentBP.totaljobcnt
		<< " while computed based on WLREPORTLOG it should be " << cnt << "********************************" << endl;
	return true;
}

//----------------------------------------------------------------------