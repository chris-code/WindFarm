#pragma once

#include <random>
#include <iostream>

#include "eval/KusiakLayoutEvaluator.h"
#include "eval/Matrix.hpp"

using namespace std;

class ES {
public:
	ES(KusiakLayoutEvaluator& evaluator, short numTurbines, float validityThreshold);
	virtual ~ES();

	bool checkTurbine(double posX, double posY, short turbinesCounter, short ignoreIndex = -1 );
	void setRandomTurbines();

	void evaluate();
	vector<long> selectParents();
	void mutateTurbines();
	void run();
private:
	KusiakLayoutEvaluator& wfle;
	double gridAnchorX;
	double gridAnchorY;
	double gridWidth;
	double gridHeight;
	
	short numTurbines;
	Matrix<double> posTurbines;
	float validityThreshold;

	default_random_engine randomEngine;
	float turbineMoveDistanceStandardDeviation = 70.0;
};
