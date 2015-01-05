#include <string>
#include <stdlib.h>
#include "eval/KusiakLayoutEvaluator.h"
#include "GA.h"

using namespace std;

int main( int argc, char **argv ) {

	if(argc < 2 + 1) {
		cout << "Usage: " << argv[0] << " scenarioPath numberOfTurbines" << endl;
		exit(EXIT_FAILURE);
	}
	string scenarioPath = argv[1];
	long numberOfTurbines = atoi(argv[2]);
	
	WindScenario ws( scenarioPath.c_str() );
	KusiakLayoutEvaluator wfle;
	wfle.initialize( ws );

	GA ga( wfle, numberOfTurbines );
	ga.run();

	return EXIT_SUCCESS;
}
