#pragma once

#include <vector>
#include <random>
#include "eval/Matrix.hpp"
#include "eval/KusiakLayoutEvaluator.h"

class Individual {
	public:
		Matrix<double> layout;
		double fitness;
		Matrix<double> turbineFitnesses;
		Individual(KusiakLayoutEvaluator *evaluator, default_random_engine *engine, Matrix<double> layout,
		           double fitness, Matrix<double> turbineFitnesses):
			wfle(evaluator), randomEngine(engine) {
			this->layout = layout;
			this->fitness = fitness;
			this->turbineFitnesses = turbineFitnesses;
		}
		Individual mutate() {
//			TODO
		}
	private:
		KusiakLayoutEvaluator *wfle;
		default_random_engine *randomEngine;
};

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
		vector<Individual> population;

		default_random_engine randomEngine;

		void evaluatePopulation();
		vector<long> selectParents();
		vector<Individual> generateOffspring(vector<long> parentIndices);
		Matrix<double> generateRandomLayout();
		bool isInObstacle(double posX, double posY);
		bool isTooClose(double posX1, double posY1, double posX2, double posY2);
};
