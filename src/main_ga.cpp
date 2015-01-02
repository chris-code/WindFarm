#include <string>
#include <stdlib.h>
#include "GA.h"

#include "eval/KusiakLayoutEvaluator.h"

using namespace std;

int main( int argc, char **argv ) {
	string scenarioPath = "Scenarios/obs_00.xml";
	if(argc >= 1 + 1) {
		scenarioPath = argv[1];
	}
	
	
	WindScenario ws( scenarioPath.c_str() );
	KusiakLayoutEvaluator wfle;
	wfle.initialize( ws );

	GA ga( wfle );
	ga.run();

	return EXIT_SUCCESS;
}
