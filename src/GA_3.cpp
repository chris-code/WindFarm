#include <algorithm>
#include <chrono>
#include "GA_3.h"

GA3::GA3(KusiakLayoutEvaluator &evaluator, long populationSize, long offspringCount, long numberOfTurbines):
	wfle(evaluator) {
	this->populationSize = populationSize;
	this->offspringCount = offspringCount;
	this->numberOfTurbines = numberOfTurbines;

	long seed = chrono::system_clock::now().time_since_epoch().count();
	randomEngine = default_random_engine(seed);

	for(long l = 0; l < populationSize; ++l) {
		population.push_back(Individual(&wfle, &randomEngine, generateRandomLayout(), -1.0, Matrix<double>(1, 1)));
	}
}

void GA3::run() {
//	TODO shrink population if necessary
	while(true) {
		evaluatePopulation();
		while(long(population.size()) > populationSize) {
			population.pop_back();
		}
		cout << "Fitness after " << wfle.getNumberOfEvaluation() << " evaluations: " << population[0].fitness << endl;
		vector<long> parentIndices = selectParents();
		vector<Individual> offspring = generateOffspring(parentIndices);
		population = offspring;
	}
}

Matrix<double> GA3::getLayout() {
	return population[0].layout; // FIXME this should be the best one
}

bool compareIndividuals(const Individual &a, const Individual &b) {
	return a.fitness > b.fitness;
}

void GA3::evaluatePopulation() {
	for(auto & individual : population) {
		wfle.evaluate(&(individual.layout));
		individual.fitness = wfle.getWakeFreeRatio();
		individual.turbineFitnesses = *(wfle.getTurbineFitnesses());
	}
	sort(population.begin(), population.end(), compareIndividuals); //TODO overwrite operator<
}

vector<long> GA3::selectParents() {
//	TODO tournament selection, maybe?
	uniform_int_distribution<long> selector(1, (populationSize * (populationSize + 1)) / 2);
	vector<long> parentIndices;
	for(long p = 0; p < offspringCount; ++p) {
		long randomValue = selector(randomEngine); //FIXME
		long sum = 0;
		for(long l = 0; l < populationSize; ++l) {
			sum += populationSize - l;
			if(randomValue <= sum) {
				parentIndices.push_back(l);
			}
		}
		parentIndices.push_back(0); // This should be unreachable
	}
	return parentIndices;
}

vector<Individual> GA3::generateOffspring(vector<long> parentIndices) {
	vector<Individual> offspring;
	for(auto p : parentIndices) {
//		TODO
		Individual i = population[p].mutate();
		offspring.push_back(i);
	}
	return offspring;
}

Matrix<double> GA3::generateRandomLayout() {
	uniform_real_distribution<double> randomPosX(0, 0 + wfle.scenario.width);
	uniform_real_distribution<double> randomPosY(0, 0 + wfle.scenario.height);

	Matrix<double> layout(numberOfTurbines, 2);
	long placedTurbines = 0;
	while(placedTurbines < numberOfTurbines) {
		double posX = randomPosX(randomEngine);
		double posY = randomPosY(randomEngine);

//		Check if we picked a location within an obstacle or too close to another turbine
		bool isValidPosition = true;
		if(isInObstacle(posX, posY)) {
			isValidPosition = false;
		}
		for(long t = 0; t < placedTurbines; ++t) {
			if(isTooClose(posX, posY, layout.get(t, 0), layout.get(t, 1))) {
				isValidPosition = false;
			}
		}

		if(isValidPosition) {
			layout.set(placedTurbines, 0, posX);
			layout.set(placedTurbines, 1, posY);
			++placedTurbines;
		}
	}
	return layout;
}

bool GA3::isInObstacle(double posX, double posY) {
	for(long o = 0; o < wfle.scenario.obstacles.rows; ++o) {
		double xmin = wfle.scenario.obstacles.get(o, 0);
		double ymin = wfle.scenario.obstacles.get(o, 1);
		double xmax = wfle.scenario.obstacles.get(o, 2);
		double ymax = wfle.scenario.obstacles.get(o, 3);
		if(posX > xmin && posY > ymin && posX < xmax && posY < ymax) {
			return true;
		}
	}
	return false;
}

bool GA3::isTooClose(double posX1, double posY1, double posX2, double posY2) {
	double distance = sqrt(pow(posX1 - posX2, 2.0) + pow(posY1 - posY2, 2.0));
	if(distance <= 8 * wfle.scenario.R) {
		return true;
	}
	return false;
}
