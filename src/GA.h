#pragma once

#include <vector>
#include "eval/Matrix.hpp"
#include "eval/WindFarmLayoutEvaluator.h"

using namespace std;

class GA {
	public:
		GA( WindFarmLayoutEvaluator &wfleInput, long numberOfTurbines );
		virtual ~GA();

		virtual void run();
	private:
		WindFarmLayoutEvaluator &wfle;
		long numberOfTurbines;
		Matrix< vector<double> > grid;
		//Matrix<char> layout; //TODO don't use char

		Matrix< vector<double> > generateGrid();
		Matrix<char> generateLayout();
		bool isValidPosition( double posX, double posY );
		vector< Matrix<char> > mate(Matrix<char> &layout, long x, long y, long offspringCount);
		Matrix<double> transformToEvalFormat(Matrix<char> &layoutToTransform);
};
