#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#define SQR(A) ((A)*(A))
#define MIN(A,B) ((A)<(B))?(A):(B)
#define MAX(A,B) ((A)>(B))?(A):(B)

using namespace std;

bool removeAtTheEnd = false;
const string probesreport = "probereport.txt";
const string jobcountfile = "jobcountprofile.txt";
const string workloadfile = "workloadreport.txt";
const string bytecountfile = "byteprofile.txt";
const string speedfile = "speedprofile.txt";

const string mvfilename = "MeansAndVariances.txt";
const string probesmeanfilename = "ProbeMeanAndVariances.txt";

class EventStream {
public:
	EventStream(const string filepath, bool valuesPerJob) : reachedEndOfStream(false), valuesArePerJob(valuesPerJob), filename(filepath) {openFile();}
	~EventStream() { fin.close(); }

	bool openFile() {
		fin.open(filename.c_str());
		if (!fin.is_open())
		{
			cerr << "Error: couldn't open jobcount file!" << endl;
			reachedEndOfStream = true;
			return false;
		}
		else {
			string line;
			getline(fin, line);
			if (fin.eof() || (0 == line.length()))
				reachedEndOfStream = true;
			reachedEndOfStream = false;
			return true;
		}
	}

	inline bool isPerJob() { return valuesArePerJob; };
	inline bool eos() { return reachedEndOfStream; }
	inline bool rewind() { fin.close(); return openFile();}

	virtual void NextStatusChange(pair <double, double>&) = 0;	// Return pair of time and value
protected:
	string filename;
	bool reachedEndOfStream;
	bool valuesArePerJob;
	ifstream fin;
	string nextline; 

	inline bool getnextline() {
		if (fin.eof() || reachedEndOfStream)
		{
			reachedEndOfStream = true;
			return false;
		}
		else
		{
			getline(fin, nextline);
			if (nextline.length() == 0) {
				cerr << "-- Reached empty line!" << endl;
				reachedEndOfStream = true;
				return false;
			}
			return true;
		}
	}
};

class PairCount: public EventStream {
public:
	PairCount(const string filepath) : EventStream(filepath, false) {}

	void NextStatusChange(pair<double, double> &time_value)
	{// Return pair of time and value
		//pair<double, double> time_value (0,0);
		time_value.first = 0;
		time_value.second = 0;

		if (getnextline()) {
			sscanf(nextline.c_str(), "%lf %lf", &time_value.first, &time_value.second);
		}
	}
};

class WorkloadReportStream : public EventStream {
public:
	WorkloadReportStream(const string filename, unsigned inx) : EventStream(filename, true), index (inx) , time (0) {}
	void NextStatusChange(pair<double, double> &time_value)
	{// Return pair of time and value
		//pair<double, double> time_value(0, 0);
		time_value.first = 0;
		time_value.second = 0;
		int id;
		time = 0;
		//double ID Size, Arrival, Departure, Energy, ExectionT, ResponseT, Slowdown, AvgSpeed;
		vector <double> values(9, 0);
		if (index >= 9) { cerr << "ERROR: out of bound index!" << endl; return; } // Guard for out of bound cases
		if (getnextline()) {
			sscanf(nextline.c_str(), "%ld %lf %lf %lf %lf %lf %lf %lf", 
				&id, &values[1], &values[2], &values[3], &values[4], &values[5], &values[6], &values[7]);
			if (values[5] != 0)	// Set avg speed experienced per job
				values[8] = values[1] / values[5];
			values[0] = id;

			time_value.first = time++;
			time_value.second = values[index];
		}
	}

	void NextReportedJob(vector<double> &values)
	{// Return pair of time and value
	 //pair<double, double> time_value(0, 0);
		//double ID Size, Arrival, Departure, Energy, ExectionT, ResponseT, Slowdown, AvgSpeed;
		int id = -1;
		values = vector <double> (9, 0);
		if (getnextline()) {
			sscanf(nextline.c_str(), "%ld %lf %lf %lf %lf %lf %lf %lf",
				&id, &values[1], &values[2], &values[3], &values[4], &values[5], &values[6], &values[7]);
			if (values[5] != 0)	// Set avg speed experienced per job
				values[8] = values[1] / values[5];

			values[0] = id;
		}
	}
private:
	unsigned index;
	double time;
};

bool compute_job_avgs(string reportfilename, string meanvarfilename) {
	cout << "Reading stream of job reports...";

	WorkloadReportStream WR(reportfilename, 0);
	vector <vector <double> > values;
	vector <vector <double> > variances;

	// Format:  JobID size Arrival Departure Energy ExecutionT ResponseT Slowdown (?Avg. Speed)
	vector< vector <double> > job_reports;
	vector<double> row;
	WR.NextReportedJob(row);
	vector <double> mean(row.size(), 0);
	vector <double> var(row.size(), 0);

	while (!WR.eos()) 
	{
		job_reports.push_back(row);

		// Format:  JobID size Arrival Departure Energy ExecutionT ResponseT Slowdown (?Avg. Speed)
		if ((mean[1] == 0) || mean[1] == row[1]) {
			mean[0]++;
			mean[1] = row[1];
			mean[4] += row[4];
			mean[5] += row[5];
			mean[6] += row[6];
			mean[7] += row[7];
			mean[8] += row[8];
		}
		else {
			for (unsigned int i = 2; i < 9 && mean[0] != 0; i++)  mean[i] /= mean[0];
			values.push_back(mean);
			mean[0] = 1;
			mean[1] = row[1];
			mean[4] = row[4];
			mean[5] = row[5];
			mean[6] = row[6];
			mean[7] = row[7];
			mean[8] = row[8];
		}
		WR.NextReportedJob(row);
	}
	for (unsigned int i = 2; i < 9 && mean[0] != 0; i++)  mean[i] /= mean[0];
	values.push_back(mean);

	unsigned int s = 0;
	mean = vector<double>(mean.size(), 0);
	for (unsigned int i = 0; i < job_reports.size() && s < values.size(); i++) 
	{
		if (job_reports[i][1] == values[s][1]) {
			mean[1] += SQR(job_reports[i][1] - values[s][1]);
			mean[4] += SQR(job_reports[i][4] - values[s][4]);
			mean[5] += SQR(job_reports[i][5] - values[s][5]);
			mean[6] += SQR(job_reports[i][6] - values[s][6]);
			mean[7] += SQR(job_reports[i][7] - values[s][7]);
			mean[8] += SQR(job_reports[i][8] - values[s][8]);
		}
		else {
			mean[0] = values[s][0];
			s++;
			for (unsigned int i = 2; i < 9 && mean[0] != 0; i++)  mean[i] /= mean[0];
			variances.push_back(mean);
			mean[1] = SQR(job_reports[i][1] - values[s][1]);
			mean[4] = SQR(job_reports[i][4] - values[s][4]);
			mean[5] = SQR(job_reports[i][5] - values[s][5]);
			mean[6] = SQR(job_reports[i][6] - values[s][6]);
			mean[7] = SQR(job_reports[i][7] - values[s][7]);
			mean[8] = SQR(job_reports[i][8] - values[s][8]);
		}
	}
	mean[0] = values[s][0];
	for (unsigned int i = 2; i < 9 && mean[0] != 0; i++)  mean[i] /= mean[0];
	variances.push_back(mean);

	ofstream probemeanvariance(meanvarfilename);
	char line[1000];
	//string S1("RV"), MEAN("Mean"), VAR("Variance"), MaxV("Max-Value"), MinV("Min-Value"), NOS("No-Of-Samples");
	//string S1("Size"), S2("mean-Energy"), S3("var-Energy"), S4("mean-ExecTime"), S5("var-ExecTime"), 
		//S6("mean-RespTime"), S7("var-RespTime"), S8("mean-Slowdown"), S9("var-Slowdown"), S10("mean-ExpSpeed var-ExpSpeed");
	sprintf(line, "%10s %20s %20s %20s %20s %20s %20s %20s %20s %20s %20s %20s", 
		"Size", "mean-Energy", "var-Energy", "mean-ExecTime", "var-ExecTime", 
		"mean-RespTime", "var-RespTime", "mean-Slowdown", "var-Slowdown", "mean-ExpSpeed", "var-ExpSpeed", "no-of-Samples");
	probemeanvariance << line << endl;
	for (unsigned int i = 0; i < values.size(); i++)
	{
		sprintf(line, "%10f %20.6lf %20.6lf %20.6lf %20.6lf %20.6lf %20.6lf %20.6lf %20.6lf %20.6lf %20.6lf %20lf", 
			values[i][1], values[i][4], variances[i][4], values[i][5], variances[i][5],
			values[i][6], variances[i][6], values[i][7], variances[i][7], values[i][8], variances[i][8], mean[0]);

		probemeanvariance << line << endl;
	}

	return true;
}

//bool compute_MeanVar(EventStream *ES, double &meanValue, double &variance, map<double, double> &ProbabilityOfValues) {
//	map <double, double> DurationAtValue;
//	cout << "Reading stream of (time,values)...";
//
//	double timestamp_next = 0, timestamp_prev = 0;
//	double value_next = 0, value_prev = 0, timelapse = 0;
//	pair<double, double> next_event = ES->NextStatusChange();
//
//	while (!ES->eos()) {
//		value_next = next_event.second;
//		timestamp_next = next_event.first;
//		timelapse = timestamp_next - timestamp_prev;
//
//		map <double, double>::iterator it = DurationAtValue.find(value_prev);
//		if (it != DurationAtValue.end()) {
//			timelapse += it->second;
//		}
//		DurationAtValue[value_prev] = timelapse;
//
//		timestamp_prev = timestamp_next;
//		value_prev = value_next;
//		next_event = ES->NextStatusChange();
//	}
//	// Should remove? 
//	if (timelapse == 1) {
//		map <double, double>::iterator it = DurationAtValue.find(value_prev);
//		if (it != DurationAtValue.end()) {
//			timelapse += it->second;
//		}
//		DurationAtValue[value_prev] = timelapse;
//	}
//	
//	double totaltime = timestamp_prev;
//	if (0 == totaltime) {
//		cerr << "Error: read the entire stream and time is still zero!" << endl;
//		return false;
//	}
//	// DurationAtValue now contains pairs of value and the sum of duration at that value
//
//	cout << "Computing the mean and probabilities..." << endl;
//
//	meanValue = 0;
//	ProbabilityOfValues = DurationAtValue;
//
//	for (map <double, double>::iterator ii = ProbabilityOfValues.begin(); ii != ProbabilityOfValues.end(); ii++) {
//		meanValue += ii->first * ii->second;
//		ii->second = ii->second / totaltime;
//		//occupancy << i << "     " << probabilityN[i] << endl;
//	}
//	meanValue = meanValue / totaltime;
//
//	cout << "Computing variance..." << endl;
//
//	double secondmoment = 0;
//
//	for (map <double, double>::iterator ii = ProbabilityOfValues.begin(); ii != ProbabilityOfValues.end(); ii++) {
//		//varianceOccupancy += probabilityN[i] * SQR(i - meanOccupancy);
//		secondmoment += ii->second * SQR(ii->first);
//	}
//	variance = secondmoment - SQR(meanValue);
//
//	cout << " done!" << endl;
//	return true;
//}

bool compute_MeanVar(EventStream *ES, double &meanValue, double &variance, map<double, double> &ProbabilityOfValues, double &maxvalue, double &minvalue, double &NoOfSamples) {
	cout << "Reading stream of (time,values)...";

	maxvalue = 0;
	minvalue = INT_MAX;
	NoOfSamples = 0;

	double totalSum = 0, totaltime = 0;
	double timestamp_next = 0, timestamp_prev = 0;
	double value_next = 0, value_prev = 0;
	pair<double, double> next_event;
	ES->NextStatusChange(next_event);

	while (!ES->eos()) {
		value_next = next_event.second;
		timestamp_next = next_event.first;

		if (ES->isPerJob()) {
			totalSum = totalSum + value_prev;
			totaltime++;
		} 
		else
		{
			totalSum = totalSum + (timestamp_next - timestamp_prev) * value_prev;
		}

		minvalue = MIN(minvalue, value_next);
		maxvalue = MAX(maxvalue, value_next);

		timestamp_prev = timestamp_next;
		value_prev = value_next;
		ES->NextStatusChange(next_event);
	}
	if (ES->isPerJob()) {
		totalSum = totalSum + value_prev;
		totaltime++;
	}
	else
	{
		totaltime = timestamp_next;
	}
	NoOfSamples = totaltime;

	if (0 == totaltime) {
		cerr << "Error: read the entire stream and time is still zero!";
		cerr << "Totalsum: " << totalSum << " Totaltime: " << totaltime << " lastTime: " << timestamp_prev << " lastValue: " << value_prev << endl;
		return false;
	}
	else
	{
		meanValue = totalSum / totaltime;
	}
	
	cout << "Computing the variance...." << endl;
	ES->rewind();

	double sumVar = 0;
	value_prev = 0;  
	timestamp_prev = 0;
	ES->NextStatusChange(next_event);
	while (!ES->eos()) {
		value_next = next_event.second;
		timestamp_next = next_event.first;

		if (ES->isPerJob()) {
			sumVar += SQR(value_prev - meanValue);
		}
		else
		{
			sumVar += SQR(value_prev - meanValue) * (timestamp_next - timestamp_prev);
		}

		timestamp_prev = timestamp_next;
		value_prev = value_next;
		ES->NextStatusChange(next_event);
	}
	if (ES->isPerJob()) {
		totalSum = totalSum + value_prev;
		totaltime++;
	}
	else
	{
		totaltime = timestamp_next;
	}
	variance = sumVar / totaltime;

	cout << " done!" << endl;
	return true;
}


bool compute_BP_MeanVar(EventStream *ES, double &meanValue, double &variance, map<double, double> &ProbabilityOfValues, double &maxvalue, double &minvalue, double &NoOfSamples) {
	cout << "Reading stream of (time,values)...";

	maxvalue = 0;
	minvalue = INT_MAX;
	NoOfSamples = 0;

	double totalSum = 0, totaltime = 0;
	double timestamp_next = 0, timestamp_prev = 0;
	double value_next = 0, value_prev = 1;
	pair<double, double> next_event;
	ES->NextStatusChange(next_event);

	unsigned bpcnt = 0;
	double bpduration = 0, bpstart = 0, bpend = 0, bpdurationsum = 0;
	vector <double> bpdurations;


	while (!ES->eos()) {
		value_next = next_event.second;
		timestamp_next = next_event.first;

		if (0 == value_prev && 0 != value_next) {
			bpend = timestamp_prev;
			bpduration = bpend - bpstart;
			minvalue = MIN(minvalue, bpduration);
			maxvalue = MAX(maxvalue, bpduration);

			bpcnt++;
			bpdurations.push_back(bpduration);
			bpdurationsum += bpduration;
			bpduration = 0;
			bpend = bpstart = timestamp_next;
		}

		timestamp_prev = timestamp_next;
		value_prev = value_next;
		ES->NextStatusChange(next_event);
	}
	bpend = timestamp_prev;
	bpduration = bpend - bpstart;
	bpdurationsum += bpduration;
	bpdurations.push_back(bpduration);
	bpcnt++;

	minvalue = MIN(minvalue, bpduration);
	maxvalue = MAX(maxvalue, bpduration);
	NoOfSamples = bpcnt;

	if (0 != bpcnt) {
		meanValue = bpdurationsum / bpcnt;
	}
	else
	{
		cerr << "Error: read the entire stream and found no busy periods!";
	}


	cout << "Computing the variance...." << endl;

	double sumVar = 0;
	value_prev = 0;
	timestamp_prev = 0;
	for (unsigned i = 0; i < bpdurations.size(); i++)
	{
		sumVar += SQR(bpdurations[i] - meanValue);
	}
	if (0 != bpdurations.size() && bpdurations.size() == bpcnt) {
		variance = sumVar / bpdurations.size();
	}
	else
	{
		cerr << "Error: read the entire stream and found no busy periods! bpdurations.size():" << bpdurations.size() << " bpcnt: " << bpcnt << endl;
	}	

	cout << " done!" << endl;
	return true;
}



int main() {
	if (true) {
		compute_job_avgs(probesreport, probesmeanfilename);

		ofstream meanvariance(mvfilename.c_str());
		string RV("RV"), MEAN("Mean"), VAR("Variance"), MaxV("Max-Value"), MinV("Min-Value"), NOS("No-of-Samples");
		char line[1000];
		sprintf(line, "%10s %20s %20s %20s %20s %20s", RV.c_str(), MEAN.c_str(), VAR.c_str(), MaxV.c_str(), MinV.c_str(), NOS.c_str());
		meanvariance << line << endl;

		double meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;

		RV = "Occupancy";
		PairCount jcnt(jobcountfile);
		meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;
		map<double, double> ProbabilityOfValuesJCNT;
		if (compute_MeanVar(&jcnt, meanValue, variance, ProbabilityOfValuesJCNT, max, min, noOfSamples)) {
			sprintf(line, "%10s %20lf %20.10lf %20.10lf %20.10lf %20.10lf", RV.c_str(), meanValue, variance, max, min, noOfSamples);
			meanvariance << line << endl;
		}
		else
			meanvariance << RV << " -1" << endl;

		RV = "Speed";
		PairCount speeds(speedfile);
		meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;
		map<double, double> ProbabilityOfValuesSpeed;
		if (compute_MeanVar(&speeds, meanValue, variance, ProbabilityOfValuesSpeed, max, min, noOfSamples)) {
			sprintf(line, "%10s %20lf %20.10lf %20.10lf %20.10lf %20.10lf", RV.c_str(), meanValue, variance, max, min, noOfSamples);
			meanvariance << line << endl;
		}
		else
			meanvariance << RV << " -1" << endl;

		RV = "Bytes";
		PairCount bytes(bytecountfile);
		meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;
		map<double, double> ProbabilityOfValuesBytes;
		if (compute_MeanVar(&bytes, meanValue, variance, ProbabilityOfValuesBytes, max, min, noOfSamples)) {
			sprintf(line, "%10s %20lf %20.10lf %20.10lf %20.10lf %20.10lf", RV.c_str(), meanValue, variance, max, min, noOfSamples);
			meanvariance << line << endl;
		}
		else
			meanvariance << RV << " -1" << endl;

		RV = "Resp.Time";
		WorkloadReportStream resptime(workloadfile, 6);
		meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;
		map<double, double> ProbabilityOfValuesRespTimes;
		if (compute_MeanVar(&resptime, meanValue, variance, ProbabilityOfValuesRespTimes, max, min, noOfSamples)) {
			sprintf(line, "%10s %20lf %20.10lf %20.10lf %20.10lf %20.10lf", RV.c_str(), meanValue, variance, max, min, noOfSamples);
			meanvariance << line << endl;
		}
		else
			meanvariance << RV << " -1" << endl;

		RV = "Slowdown";
		WorkloadReportStream slowdown(workloadfile, 7);
		meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;
		map<double, double> ProbabilityOfValuesSL;
		if (compute_MeanVar(&slowdown, meanValue, variance, ProbabilityOfValuesSL, max, min, noOfSamples)) {
			sprintf(line, "%10s %20lf %20.10lf %20.10lf %20.10lf %20.10lf", RV.c_str(), meanValue, variance, max, min, noOfSamples);
			meanvariance << line << endl;
		}
		else
			meanvariance << RV << " -1" << endl;

		RV = "Energy";
		WorkloadReportStream energy(workloadfile, 4);
		meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;
		map<double, double> ProbabilityOfValuesEng;
		if (compute_MeanVar(&energy, meanValue, variance, ProbabilityOfValuesEng, max, min, noOfSamples)) {
			sprintf(line, "%10s %20lf %20.10lf %20.10lf %20.10lf %20.10lf", RV.c_str(), meanValue, variance, max, min, noOfSamples);
			meanvariance << line << endl;
		}
		else
			meanvariance << RV << " -1" << endl;

		RV = "Size";
		WorkloadReportStream size(workloadfile, 1);
		meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;
		map<double, double> ProbabilityOfValuesSize;
		if (compute_MeanVar(&size, meanValue, variance, ProbabilityOfValuesSize, max, min, noOfSamples)) {
			sprintf(line, "%10s %20lf %20.10lf %20.10lf %20.10lf %20.10lf", RV.c_str(), meanValue, variance, max, min, noOfSamples);
			meanvariance << line << endl;
		}
		else
			meanvariance << RV << " -1" << endl;

		RV = "RcvdSpeed";
		WorkloadReportStream rcvdspeed(workloadfile, 8);
		meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;
		map<double, double> ProbabilityOfValuesRCVSpeed;
		if (compute_MeanVar(&rcvdspeed, meanValue, variance, ProbabilityOfValuesSize, max, min, noOfSamples)) {
			sprintf(line, "%10s %20lf %20.10lf %20.10lf %20.10lf %20.10lf", RV.c_str(), meanValue, variance, max, min, noOfSamples);
			meanvariance << line << endl;
		}
		else
			meanvariance << RV << " -1" << endl;


		RV = "BPLength";
		PairCount bpl(jobcountfile);
		meanValue = 0, variance = 0, min = 0, max = 0, noOfSamples = 0;
		map<double, double> ProbabilityOfValuesBPL;
		if (compute_BP_MeanVar(&bpl, meanValue, variance, ProbabilityOfValuesBPL, max, min, noOfSamples)) {
			sprintf(line, "%10s %20lf %20.10lf %20.10lf %20.10lf %20.10lf", RV.c_str(), meanValue, variance, max, min, noOfSamples);
			meanvariance << line << endl;
		}
		else
			meanvariance << RV << " -1" << endl;
	}
	if (removeAtTheEnd == true) {
		ofstream file;
		file.open(probesreport.c_str());
		file.close();
		remove(probesreport.c_str());
		file.open(jobcountfile.c_str());
		file.close();
		remove(jobcountfile.c_str());
		file.open(workloadfile.c_str());
		file.close();
		remove(workloadfile.c_str());
		file.open(bytecountfile.c_str());
		file.close();
		remove(bytecountfile.c_str());
		file.open(speedfile.c_str());
		file.close();
		remove(speedfile.c_str());
	}
		
	return 0;
}
