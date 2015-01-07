#include <iostream>
#include <unistd.h>

#include "eval/WindScenario.h"
#include "eval/KusiakLayoutEvaluator.h"
#include "GA_1.h"

using namespace std;

void doSingleRun(string scenarioPath, long numberOfTurbines, float validityThreshold) {
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
}

vector< pair<string, float> > getScenarioPaths() {
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

void doFullRun(long numberOfTurbines) {
	vector< pair<string, float> > scenarios = getScenarioPaths();
	for (auto s: scenarios) {
		doSingleRun(s.first, numberOfTurbines, s.second);
	}
}

int main(int argc, char **argv) {
	string scenarioPath = "Scenarios/obs_00.xml";
	long numberOfTurbines = 400;
	float validityThreshold = 0.8;
	bool fullRun = false;
	char option;
	while ((option = getopt(argc, argv, "t:v:s:f")) != -1) {
		switch (option) {
			case 't':
				numberOfTurbines = std::atoi(optarg);
				break;
			case 'v':
				validityThreshold = std::atof(optarg);
				break;
			case 's':
				scenarioPath = optarg;
				break;
			case 'f':
				fullRun = true;
				break;
			case '?':
				if(optopt == 't' || optopt == 'v' || optopt == 's') {
					std::cerr << "Option -%" << optopt << " requires an argument." << std::endl;
				}
				else {
					std::cerr << "Unknown option " << optopt << std::endl;
				}
				exit(EXIT_FAILURE);
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}

	cout << "Evolution Strategy WindFLO" << endl << endl;	
	if(fullRun) {
		doFullRun(numberOfTurbines);
	}
	else {
		doSingleRun(scenarioPath, numberOfTurbines, validityThreshold);
	}
	return EXIT_SUCCESS;
}
