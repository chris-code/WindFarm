#include <fstream>
#include <iostream>
#include <unistd.h>

#include "eval/Matrix.hpp"
#include "eval/WindScenario.h"
#include "eval/KusiakLayoutEvaluator.h"
#include "GA_1.h"

using namespace std;

void writeLayoutToDisc(Matrix<double> layout, string path) {
	ofstream outputFile;
	outputFile.open(path);
	if(outputFile.is_open()) {
		outputFile << "#posX" << "\t" << "posY" << endl;
		for(long t = 0; t < layout.rows; ++t) {
			outputFile << layout.get(t, 0) << "\t" << layout.get(t, 1) << endl;
		}
		outputFile.close();
	} else {
		cerr << "ERROR: Could not open " << path << " for writing" << endl;
	}
}

Matrix<double> doSingleRun(string scenarioPath, long numberOfTurbines, float validityThreshold) {
	cout << "Scenario: " << scenarioPath << endl;
	cout << "Number of turbines: " << numberOfTurbines << endl;
	cout << "Minimal wake free ratio: " << validityThreshold << endl;

	// Load wind scenario
	WindScenario wsc(scenarioPath);

	// Initialize evaluator
	KusiakLayoutEvaluator wfle;
	wfle.initialize(wsc);

	// Initialize and run ES algorithm
	ES es(wfle, numberOfTurbines, validityThreshold);
	es.run();
	return es.getLayout();
}

vector< pair<string, float> > getScenarios() {
	vector< pair<string, float> > scenarioPaths;
	scenarioPaths.push_back(make_pair("Scenarios/00.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/01.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/02.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/03.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/04.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/05.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/06.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/07.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/08.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/09.xml", 0.85));
	scenarioPaths.push_back(make_pair("Scenarios/obs_00.xml", 0.80));
	scenarioPaths.push_back(make_pair("Scenarios/obs_01.xml", 0.80));
	scenarioPaths.push_back(make_pair("Scenarios/obs_02.xml", 0.80));
	scenarioPaths.push_back(make_pair("Scenarios/obs_03.xml", 0.80));
	scenarioPaths.push_back(make_pair("Scenarios/obs_04.xml", 0.80));
	scenarioPaths.push_back(make_pair("Scenarios/obs_05.xml", 0.80));
	scenarioPaths.push_back(make_pair("Scenarios/obs_06.xml", 0.80));
	scenarioPaths.push_back(make_pair("Scenarios/obs_07.xml", 0.80));
	scenarioPaths.push_back(make_pair("Scenarios/obs_08.xml", 0.80));
	scenarioPaths.push_back(make_pair("Scenarios/obs_09.xml", 0.80));
	return scenarioPaths;
}

void doFullRun(long numberOfTurbines, string outputDirectory, bool writeToDisk) {
	vector< pair<string, float> > scenarios = getScenarios();
	for(auto s : scenarios) {
		Matrix<double> layout = doSingleRun(s.first, numberOfTurbines, s.second);
		if(writeToDisk) {
			string outputFilePath = outputDirectory + s.first.substr(s.first.find_last_of('/') + 1);
			ofstream outputFile;
			outputFile.open(outputFilePath);
			if(outputFile.is_open()) {
				outputFile << "#posX" << "\t" << "posY" << endl;
				for(long t = 0; t < layout.rows; ++t) {
					outputFile << layout.get(t, 0) << "\t" << layout.get(t, 1) << endl;
				}
				outputFile.close();
			} else {
				cerr << "ERROR: Could not open " << outputFilePath << " for writing" << endl;
			}
		}
	}
}

int main(int argc, char **argv) {
	string scenarioPath;
	string outputPath;
	bool writeToDisc = false;
	long numberOfTurbines = 400;
	float validityThreshold = 0.8;
	bool validitySpecified = false;
	bool singleRun = false;
	bool fullRun = false;
	char option;
	while((option = getopt(argc, argv, "t:v:s:w:f")) != -1) {
		switch(option) {
			case 't':
				numberOfTurbines = std::atoi(optarg);
				break;
			case 'v':
				if(fullRun) {
					cerr << "Error: -v and -f are mutually exclusive" << endl;
					exit(EXIT_FAILURE);
				}
				validitySpecified = true;
				validityThreshold = std::atof(optarg);
				break;
			case 's':
				if(fullRun) {
					cerr << "Error: -s and -f are mutually exclusive" << endl;
					exit(EXIT_FAILURE);
				}
				singleRun = true;
				scenarioPath = optarg;
				break;
			case 'w':
				writeToDisc = true;
				outputPath = optarg;
				break;
			case 'f':
				if(singleRun) {
					cerr << "Error: options -s and -f are mutually exclusive" << endl;
					exit(EXIT_FAILURE);
				}
				if(validitySpecified) {
					cerr << "Error: -v and -f are mutually exclusive" << endl;
					exit(EXIT_FAILURE);
				}
				fullRun = true;
				break;
			case '?':
				if(optopt == 't' || optopt == 'v' || optopt == 's') {
					cerr << "Option -%" << optopt << " requires an argument." << endl;
				} else {
					cerr << "Unknown option " << optopt << endl;
				}
				exit(EXIT_FAILURE);
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}

	cout << "Evolution Strategy WindFLO" << endl << endl;
	if(fullRun) {
		doFullRun(numberOfTurbines, outputPath, writeToDisc);
	} else {
		Matrix<double> optimalLayout = doSingleRun(scenarioPath, numberOfTurbines, validityThreshold);
		if(writeToDisc) {
			writeLayoutToDisc(optimalLayout, outputPath);
		}
	}
	return EXIT_SUCCESS;
}
