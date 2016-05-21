/***
* PRandTest.cpp - The tester for PRandGen
*
*       Written by Maryam Elahi
*		Last updated: Dec, 2012
*
* Purpose:
*       Tester.
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

#include "PRandomGen.h"
#include "WLGenExponential.h"
#include "WLGenPareto.h"

using namespace std;

//----------------------------------------------------------------------

const string rand_file = "..\\..\\data\\inputs\\100MBRandomBytes-29NOV2012.dat";



//----------------------------------------------------------------------

int main (int argc, int* argv) {

	// Start of a new simulation: say hi!
	cout << "=========================================================" << endl
		 << "Welcom to S3: The Speed Scaling Simulation!" << endl
		 << "=========================================================" << endl
		 << endl;

	yLog::setLogFile("Errors.txt",ERRORLOG);

	//PRandomGen	pran(rand_file);
	//PRandomGen	pran((unsigned int) time(0));
	PRandomGen::setupGen(1982);
	
	
	//ofstream fout2("fracfrandom100k.txt");
	//for (int i = 0; i < 10000; i++)
	//	fout << PRandomGen::getUrandFrac() << endl;

	//PRandomGen::reset();
	//ofstream fout2("fracfrandom100k2.txt");

	//for (int i = 0; i < 10000; i++)
	//	fout2 << PRandomGen::getUrandFrac() << endl;

	WLGenExponential wl(0.5, 2, 100000);
	ofstream fout("EXP10k--Arr_0.5--meanSize_2.txt");
	for (int i = 0; i < 100000; i++) {
		Job j = ((ArrivalEvent*)wl.next_arrival())->job_m;
		fout << j.getID() << "\t" << j.getArrival() << "\t" << j.getSize() << endl;
	}

	WLGenPareto wlp(0.5, 2, 100000);
	ofstream fout2("PRT10k--Arr_0.5--meanSize_2.txt");
	for (int i = 0; i < 100000; i++) {
		Job j = ((ArrivalEvent*)wlp.next_arrival())->job_m;
		fout2 << j.getID() << "\t" << j.getArrival() << "\t" << j.getSize() << endl;
	}
	return 0;
}

//----------------------------------------------------------------------


