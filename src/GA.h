#pragma once

#include <vector>
#include "eval/Matrix.hpp"
#include "eval/WindFarmLayoutEvaluator.h"

using namespace std;

class GA {
	public:
		GA( WindFarmLayoutEvaluator &wfleInput );
		virtual ~GA();

		virtual void run();
	private:
		WindFarmLayoutEvaluator &wfle;
		Matrix<double> grid;
		long numberOfLayouts;
		
		Matrix<double> generateGrid();
		bool isValidPosition(double posX, double posY);
};
