#ifndef __GA_H__
#define __GA_H__

#include "eval/WindScenario.h"
#include "eval/KusiakLayoutEvaluator.h"
#include "eval/Matrix.hpp"

class GA {
  public:
    KusiakLayoutEvaluator wfle;
    Matrix<int>* pops;
    int num_pop;
    double* fits;
    int nt;
    int tour_size;
    double mut_rate;
    double cross_rate;
    Matrix<double>* grid;

    GA(KusiakLayoutEvaluator evaluator);
    ~GA();
    void run();

  private:
    void evaluate();
};

#endif
