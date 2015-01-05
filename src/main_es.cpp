#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "eval/WindScenario.h"
#include "eval/KusiakLayoutEvaluator.h"
#include "ES.h"

using namespace std;

int main(int argc, char **argv) {
	cout << "Evolution Strategy WindFLO" << endl << endl;

	// Initialize random number generator
	srand( time( NULL ) );

	// Load wind scenario
	WindScenario wsc("Scenarios/00.xml");

	// Initialize evaluator
	KusiakLayoutEvaluator wfle;
	wfle.initialize(wsc);

	// Initialize and run ES algorithm
	ES es(wfle, 400);
	es.run();

	return EXIT_SUCCESS;
}
