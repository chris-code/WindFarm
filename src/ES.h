#pragma once

#include <random>
#include <iostream>

#include "eval/KusiakLayoutEvaluator.h"
#include "eval/WindFarmLayoutEvaluator.h"
#include "eval/Matrix.hpp"

using namespace std;

class ES {
public:
	ES(WindFarmLayoutEvaluator& evaluator, short numTurbines, float validityThreshold);
	virtual ~ES();

	bool checkTurbinePosition(double posX, double posY, short turbinesCounter, short ignoreIndex = -1 );
	void countInvalidTurbines();
	void setRandomTurbines();

	void evaluate();
	vector<long> selectParents();
	void mutateTurbines();
	void run();
private:
	WindFarmLayoutEvaluator& wfle;
	double gridAnchorX;
	double gridAnchorY;
	double gridWidth;
	double gridHeight;
	
	short numTurbines;
	Matrix<double> posTurbines;
	float validityThreshold;
	long numberOfInvalidTurbines;

	default_random_engine randomEngine;
	float turbineMoveDistanceStandardDeviation = 70.0;
};
