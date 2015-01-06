#include <iostream>
#include <unistd.h>

#include "eval/WindScenario.h"
#include "eval/KusiakLayoutEvaluator.h"
#include "ES.h"

using namespace std;

int main(int argc, char **argv) {
	string scenarioPath = "Scenarios/obs_00.xml";
	long numberOfTurbines = 400;
	float validityThreshold = 0.8;
	char option;
	while ((option = getopt(argc, argv, "t:v:s:")) != -1) {
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
	cout << "Scenario: " << scenarioPath << endl;
	cout << "Number of turbines: " << numberOfTurbines << endl;
	cout << "Minimal wake free ratio: " << validityThreshold << endl;

	// Initialize random number generator
	srand( time( NULL ) );

	// Load wind scenario
	WindScenario wsc(scenarioPath);

	// Initialize evaluator
	KusiakLayoutEvaluator wfle;
	wfle.initialize(wsc);

	// Initialize and run ES algorithm
	ES es(wfle, numberOfTurbines, validityThreshold);
	es.run();

	return EXIT_SUCCESS;
}
