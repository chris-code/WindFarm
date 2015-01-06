#pragma once

#include <vector>
#include <cstdlib>
#include <stdlib.h>

#include "eval/KusiakLayoutEvaluator.h"
#include "eval/Matrix.hpp"

class ES {
public:
	ES(KusiakLayoutEvaluator& evaluator, short numTurbines, float validityThreshold);
	virtual ~ES();

	bool checkTurbine(double posX, double posY, short turbinesCounter, short ignoreIndex = -1 );
	void setRandomTurbines();

	void run();
	void evaluate();

	// HELPER FUNCTIONS, MAY BE MOVED TO AN INDEPENDENT CLASS
	double randInt(double from, double to);

private:
	double gridAnchorX;
	double gridAnchorY;
	double gridWidth;
	double gridHeight;

	KusiakLayoutEvaluator& wfle;
	Matrix<double> mutMat;
	short numTurbines;
	float validityThreshold;
	Matrix<double> posTurbines;
};
