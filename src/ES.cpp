#include "ES.h"

ES::ES(KusiakLayoutEvaluator& evaluator, short numTurbines, float validityThreshold) :
		wfle(evaluator) {
	this->numTurbines = numTurbines;
	this->posTurbines = Matrix<double>(numTurbines, 2);
	this->validityThreshold = validityThreshold;

	gridAnchorX = 0;
	gridAnchorY = 0;
	gridWidth = wfle.scenario.width;
	gridHeight = wfle.scenario.height;
}

ES::~ES() {
}

bool ES::checkTurbine(double posX, double posY, short turbinesCounter,
		short ignoreIndex) {
	// check whether posX and posY are within the range of the grid
	if (posX < gridAnchorX || posY < gridAnchorY
			|| posX > gridAnchorX + gridWidth
			|| posY > gridAnchorY + gridHeight)
		return false;

	// check whether turbine is not within an obstacle
	for (unsigned int o = 0; o < wfle.scenario.obstacles.rows; o++) {
		double xmin = wfle.scenario.obstacles.get(o, 0);
		double ymin = wfle.scenario.obstacles.get(o, 1);
		double xmax = wfle.scenario.obstacles.get(o, 2);
		double ymax = wfle.scenario.obstacles.get(o, 3);

		if (posX > xmin && posY > ymin && posX < xmax && posY < ymax) {
			return false;
		}
	}

	// check whether turbine is not to close to an other turbine
	for (short i = 0; i < turbinesCounter; i++) {
		if (i == ignoreIndex)
			continue;
		double dist = sqrt(
				pow(posX - posTurbines(i, 0), 2)
						+ pow(posY - posTurbines(i, 1), 2));
		if (dist <= 8 * wfle.scenario.R) {
			return false;
		}
	}

	return true;
}

void ES::setRandomTurbines() {
	short turbinesCounter = 0;

	while (turbinesCounter < numTurbines) {
		// create turbine at random position
		double posX = randInt(gridAnchorX, gridAnchorX + gridWidth);
		double posY = randInt(gridAnchorY, gridAnchorY + gridHeight);

		if (checkTurbine(posX, posY, turbinesCounter)) {
			posTurbines.set(turbinesCounter, 0, posX);
			posTurbines.set(turbinesCounter, 1, posY);

			turbinesCounter++;
		}
	}
}

void ES::run() {
	setRandomTurbines();

	evaluate();

	// 999 evaluations
	for (int i = 0; i < 999; i++) {
		Matrix<double> *fitnesses = wfle.getTurbineFitnesses();

		for (short j = 0; j < numTurbines; j++) {
			if (fitnesses->get(j, 0) < validityThreshold) {
				double newPosX = posTurbines(j, 0) + randInt(-100, 100);
				double newPosY = posTurbines(j, 1) + randInt(-100, 100);

				if (checkTurbine(newPosX, newPosY, numTurbines, j)) {
					posTurbines(j, 0) = newPosX;
					posTurbines(j, 1) = newPosY;
				}
			}
		}

		delete fitnesses;

		evaluate();
	}
}

void ES::evaluate() {
	double validTurbines = 0.0;

	double fitness = wfle.evaluate(&posTurbines);

	Matrix<double> *fitnesses = wfle.getTurbineFitnesses();

	for (int i = 0; i < numTurbines; i++) {
		if (fitnesses->get(i, 0) > validityThreshold) {
			validTurbines++;
		}
	}

	delete fitnesses;

	if (wfle.getNumberOfEvaluation() % 10 == 1)
		cout << "Valid turbines after " << wfle.getNumberOfEvaluation()
				<< " iterations: " << validTurbines << " with fitness "
				<< fitness << endl;
}

double ES::randInt(double from, double to) {
	double randNum = double(rand()) / RAND_MAX;
	randNum *= to - from;
	return (randNum + from);
}
