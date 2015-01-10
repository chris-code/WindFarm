#include <algorithm>
#include <chrono>
#include "GA_1.h"

GA::GA(WindFarmLayoutEvaluator& evaluator, short numTurbines, float validityThreshold) :
	wfle(evaluator) {
	this->numTurbines = numTurbines;
	this->posTurbines = Matrix<double>(numTurbines, 2);
	this->validityThreshold = validityThreshold;

	gridAnchorX = 0;
	gridAnchorY = 0;
	gridWidth = dynamic_cast<KusiakLayoutEvaluator*>(&wfle)->scenario.width;
	gridHeight = dynamic_cast<KusiakLayoutEvaluator*>(&wfle)->scenario.height;

	long seed = chrono::system_clock::now().time_since_epoch().count();
	randomEngine = default_random_engine(seed);
}

GA::~GA() {
}

bool GA::checkTurbinePosition(double posX, double posY, short turbinesCounter,
                              short ignoreIndex) {
	// check whether posX and posY are within the range of the grid
	if(posX < gridAnchorX || posY < gridAnchorY || posX > gridAnchorX + gridWidth
	        || posY > gridAnchorY + gridHeight)
		return false;

	// check whether turbine is not within an obstacle
	KusiakLayoutEvaluator *kusiakPointer = dynamic_cast<KusiakLayoutEvaluator*>(&wfle);
	for(unsigned int o = 0; o < kusiakPointer->scenario.obstacles.rows; o++) {
		double xmin = kusiakPointer->scenario.obstacles.get(o, 0);
		double ymin = kusiakPointer->scenario.obstacles.get(o, 1);
		double xmax = kusiakPointer->scenario.obstacles.get(o, 2);
		double ymax = kusiakPointer->scenario.obstacles.get(o, 3);
		if(posX > xmin && posY > ymin && posX < xmax && posY < ymax) {
			return false;
		}
	}

	// check whether turbine is not to close to an other turbine
	for(long i = 0; i < turbinesCounter; i++) {
		if(i == ignoreIndex)
			continue;
		double dist = sqrt(pow(posX - posTurbines(i, 0), 2) + pow(posY - posTurbines(i, 1), 2));
		if(dist <= 8 * kusiakPointer->scenario.R) {
			return false;
		}
	}

	return true;
}

void GA::countInvalidTurbines() {
	numberOfInvalidTurbines = 0;
	Matrix<double> *fitnesses = wfle.getTurbineFitnesses();
	for(int i = 0; i < numTurbines; i++) {
		if(fitnesses->get(i, 0) <= validityThreshold) {
			++numberOfInvalidTurbines;
		}
	}
	delete fitnesses;
}

void GA::setRandomTurbines() {
	long turbinesCounter = 0;

	// Try to equally space turbines
	float equalSpacingDistance = sqrt(gridWidth * gridHeight / double(numTurbines));
	float minSpacingDistance = 13.0 * dynamic_cast<KusiakLayoutEvaluator*>(&wfle)->scenario.R;
	float spacingDistance = max(minSpacingDistance, equalSpacingDistance);
	for(float posY = 0; posY < gridHeight; posY += spacingDistance * 1.5) {
		for(float posX = 0; posX < gridWidth; posX += spacingDistance * 1.5) {
			if(turbinesCounter == numTurbines) {
				return;
			}
			if(checkTurbinePosition(posX, posY, turbinesCounter)) {
				posTurbines.set(turbinesCounter, 0, posX);
				posTurbines.set(turbinesCounter, 1, posY);
				++turbinesCounter;
			}
		}
	}

	// Place leftover turbines randomly
	uniform_int_distribution<long> randomPosX(gridAnchorX, gridAnchorX + gridWidth);
	uniform_int_distribution<long> randomPosY(gridAnchorY, gridAnchorY + gridHeight);
	while(turbinesCounter < numTurbines) {
		// create turbine at random position
		double posX = randomPosX(randomEngine);
		double posY = randomPosY(randomEngine);

		if(checkTurbinePosition(posX, posY, turbinesCounter)) {
			posTurbines.set(turbinesCounter, 0, posX);
			posTurbines.set(turbinesCounter, 1, posY);

			turbinesCounter++;
		}
	}
}

void GA::evaluate() {
	double fitness = wfle.evaluate(&posTurbines);
	countInvalidTurbines();

	if(wfle.getNumberOfEvaluation() % 10 == 1 || wfle.getNumberOfEvaluation() == 1000)
		cout << "Iteration " << wfle.getNumberOfEvaluation() << ":\t" << numberOfInvalidTurbines
		     << " invalid turbines, global wake free ratio:\t" << fitness << endl;
}

bool compareFitness(pair<double, long> fitness1, pair<double, long> fitness2) {
	// return true if the first pair is smaller than the second one
	return fitness1.first > fitness2.first; // We want the highest fitness to have the lowest rank
}

vector<long> GA::selectParents() {
	vector<long> parents;
	Matrix<double> *fitnesses = wfle.getTurbineFitnesses();
	for(long t = 0; t < fitnesses->rows; ++t) {
		if(fitnesses->get(t, 0) < validityThreshold) {
			parents.push_back(t);
		}
	}
	delete fitnesses;

	return parents;

//	uniform_int_distribution<long> randomSelector(1, numTurbines * (numTurbines + 1) / 2);
//
//	Matrix<double> *fitnesses = wfle.getTurbineFitnesses();
//	vector< pair<double, long> > sortedFitnesses;
//	for (long t=0; t<fitnesses->rows; ++t) {
//		sortedFitnesses.push_back( make_pair(fitnesses->get(t, 0), t) );
//	}
//	delete fitnesses;
//	sort(sortedFitnesses.begin(), sortedFitnesses.end(), compareFitness);
//
//	long additionalParentCount = 5;
//	vector<long> parents;
//	for(long p = numTurbines; p > numTurbines - numberOfInvalidTurbines; --p) {
//		parents.push_back(sortedFitnesses[p-1].second);
//	}
//
//	for(long p = 0; p < additionalParentCount; ++p) {
//		long randomValue = randomSelector(randomEngine);
//		for(long v = 1; v <= numTurbines; ++v) {
//			long g = v * (v + 1) / 2;
//			if(randomValue <= g) {
//				parents.push_back(sortedFitnesses[v-1].second);
//				break;
//			}
//		}
//	}
//
//	return parents;
}

void GA::mutateTurbines() {
	normal_distribution<float> turbineMoveDistribution(0, turbineMoveDistanceStandardDeviation);

	vector<long> parents = selectParents();
	for(auto &p : parents) {
		double newPosX = posTurbines(p, 0) + turbineMoveDistribution(randomEngine);
		double newPosY = posTurbines(p, 1) + turbineMoveDistribution(randomEngine);
		if(checkTurbinePosition(newPosX, newPosY, numTurbines, p)) {
			posTurbines(p, 0) = newPosX;
			posTurbines(p, 1) = newPosY;
		}
	}
}

void GA::run() {
	setRandomTurbines();

	// 1000 evaluations
	for(long i = 0; i < 1000; i++) {
		evaluate();
		if(numberOfInvalidTurbines == 0) {
			break;
		}
		mutateTurbines();
	}
}

Matrix<double> GA::getLayout() {
	return posTurbines;
}
