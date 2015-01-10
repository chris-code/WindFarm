#pragma once

#include <vector>
#include <random>
#include "eval/Matrix.hpp"
#include "eval/KusiakLayoutEvaluator.h"

class GA3 {
	public:
		GA3(KusiakLayoutEvaluator &evaluator, long populationSize, long offspringCount, long numberOfTurbines);

		void run();
		Matrix<double> getLayout();
	private:
		KusiakLayoutEvaluator& wfle;
		long populationSize;
		long offspringCount;
		long numberOfTurbines;
		vector< pair < Matrix<double>, double> > population;

		default_random_engine randomEngine;

		void evaluatePopulation();
		vector<long> selectParents();
		Matrix<double> generateRandomLayout();
		bool isInObstacle(double posX, double posY);
		bool isTooClose(double posX1, double posY1, double posX2, double posY2);
};
