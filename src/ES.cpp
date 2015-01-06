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
	
	randomEngine = default_random_engine(); //TODO needs initialization?
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
	uniform_int_distribution<long> randomPosX(gridAnchorX, gridAnchorX + gridWidth);
	uniform_int_distribution<long> randomPosY(gridAnchorY, gridAnchorY + gridHeight);
	
	short turbinesCounter = 0;
	while (turbinesCounter < numTurbines) {
		// create turbine at random position
		double posX = randomPosX(randomEngine);
		double posY = randomPosY(randomEngine);

		if (checkTurbine(posX, posY, turbinesCounter)) {
			posTurbines.set(turbinesCounter, 0, posX);
			posTurbines.set(turbinesCounter, 1, posY);

			turbinesCounter++;
		}
	}
}

void ES::evaluate() {
	double fitness = wfle.evaluate(&posTurbines);

	long validTurbines = 0;
	Matrix<double> *fitnesses = wfle.getTurbineFitnesses();
	for (int i = 0; i < numTurbines; i++) {
		if (fitnesses->get(i, 0) > validityThreshold) {
			++validTurbines;
		}
	}
	delete fitnesses;

	if (wfle.getNumberOfEvaluation() % 10 == 1)
		cout << "Iteration " << wfle.getNumberOfEvaluation() << ":\t" << validTurbines
		<< " valid turbines, global wake free ratio:\t" << fitness << endl;
}

void ES::mutateTurbines() {
	normal_distribution<float> turbineMoveDistribution(0, turbineMoveDistanceStandardDeviation);
	
	Matrix<double> *fitnesses = wfle.getTurbineFitnesses();
	for (short j = 0; j < numTurbines; j++) {
		if (fitnesses->get(j, 0) < validityThreshold) {
			double newPosX = posTurbines(j, 0) + turbineMoveDistribution(randomEngine);
			double newPosY = posTurbines(j, 1) + turbineMoveDistribution(randomEngine);
			if (checkTurbine(newPosX, newPosY, numTurbines, j)) {
				posTurbines(j, 0) = newPosX;
				posTurbines(j, 1) = newPosY;
			}
		}
	}
	delete fitnesses;
}

void ES::run() {
	setRandomTurbines();
	
	// 1000 evaluations
	for (int i = 0; i < 1000; i++) {
		evaluate();
		mutateTurbines();
	}
}
