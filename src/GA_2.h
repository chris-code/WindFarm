#pragma once

#include <vector>
#include "eval/Matrix.hpp"
#include "eval/WindFarmLayoutEvaluator.h"

using namespace std;

class GA2 {
	public:
		GA2( WindFarmLayoutEvaluator &wfleInput, long numberOfTurbines );
		virtual ~GA2();

		virtual void run();
		Matrix<double> getLayout();
	private:
		WindFarmLayoutEvaluator &wfle;
		long numberOfTurbines;
		Matrix< vector<double> > grid;
		Matrix<char> layout; //TODO don't use char

		Matrix< vector<double> > generateGrid();
		Matrix<char> generateLayout();
		bool isValidPosition( double posX, double posY );
		long countInvalidTurbines(double threshold);
		vector<pair<long, long> > selectParents(Matrix<double> &evalLayout, long howMany);
		vector< Matrix<char> > mate(long x, long y, long offspringCount);
		vector< Matrix<char> > mate(vector< pair<long, long> > turbines, long offspringCount);
		Matrix<double> transformToEvalFormat(Matrix<char> &layoutToTransform);
};
