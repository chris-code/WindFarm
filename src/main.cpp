#include <fstream>
#include <iostream>
#include <getopt.h>

#include "eval/Matrix.hpp"
#include "eval/WindScenario.h"
#include "eval/KusiakLayoutEvaluator.h"
#include "GA_1.h"
#include "GA_2.h"

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

	// Initialize and run GA algorithm
	char c = '1';
	switch(c) {
		case '1': {
			GA ea(wfle, numberOfTurbines, validityThreshold);
			ea.run();
			return ea.getLayout();
		}
		case '2': {
			GA2 ea2(wfle, numberOfTurbines);
			ea2.run();
			return ea2.getLayout();
		}
		default:
			cerr << "'" << c << "' in switch construct." << endl;
			return nullptr;
	}
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

void printHelp(char **argv) {
	cout << "SYNOPSIS" << endl;
	cout << "\t" << argv[0] << " -f" << " [-t turbines]" << " [-w path]" << endl;
	cout << "\t" << argv[0] << " -s scenario" << " [-t turbines]" << " [-v validity-threshold]" << " [-w path]" << endl;
	cout << "\t" << argv[0] << " -h|--help" << endl;

	cout << "OPTIONS" << endl;
	cout << "\t" << "-f, --full-run" << endl;
	cout << "\t\t" << "Perform full run (optimize all scenarios in ./Scenarios). Mutually exclusive with -s and -v."
	     << endl;

	cout << "\t" << "-s path, --scenario path" << endl;
	cout << "\t\t" << "Optimize scenario described in $path. Mutually exclusive with -f" << endl;

	cout << "\t" << "-t int, --turbines int" << endl;
	cout << "\t\t" << "Number of turbines to place on the grid." << endl;

	cout << "\t" << "-v float, --validity-threshold float" << endl;
	cout << "\t\t" << "Wake free ratio [0..1] a turbine needs to achieve to be considered valid.";
	cout << "Mutually exclusive with -f" << endl;

	cout << "\t" << "-w path, --write-to-disc path" << endl;
	cout << "\t\t" << "Write solution to a file in gnuplot compatible format." << endl;
	cout << "\t\t" << "If -f is specified, $path should be a directory where the solutions will be placed." << endl;
	cout << "\t\t" << "If -s is specified, $path should be a filename where the solution will be written to." << endl;

	cout << "\t" << "-h, --help" << endl;
	cout << "\t\t" << "Display this message." << endl;

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

	struct option options[] = {
		{"turbines", required_argument, nullptr, 't'},
		{"validity-threshold", required_argument, nullptr, 'v'},
		{"scenario", required_argument, nullptr, 's'},
		{"write-to-disc", required_argument, nullptr, 'w'},
		{"full-run", no_argument, nullptr, 'f'},
		{"help", no_argument, nullptr, 'h'},
		{0, 0, nullptr, 0}
	};
	char option;
	while((option = getopt_long(argc, argv, "t:v:s:w:fh", options, nullptr)) != -1) {
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
			case 'h':
				printHelp(argv);
				exit(EXIT_SUCCESS);
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
	if(!singleRun && !fullRun) {
		cerr << "Error: Specify either -s or -f" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Genetic Algorithm WindFLO" << endl << endl;
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
