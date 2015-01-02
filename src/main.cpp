//
//  main.cpp
//

#include <iostream>
#include <stdio.h>

#include "eval/WindScenario.h"
#include "GA.h"
#include "eval/KusiakLayoutEvaluator.h"

int main(int argc, const char * argv[]) {

  WindScenario wsc("Scenarios/obs_00.xml");
  KusiakLayoutEvaluator wfle;
  wfle.initialize(wsc);
  GA ga(wfle);
  ga.run();
}
