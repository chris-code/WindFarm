#include <algorithm>
#include <chrono>
#include "GA_3.h"

//	======================================== PUBLIC MEMBERS ========================================

GA3::GA3(KusiakLayoutEvaluator &evaluator, bool commaSelection, long populationSize, long offspringCount,
         long numberOfTurbines, double validityThreshold):
	wfle(evaluator) {
	if(commaSelection && offspringCount < populationSize) {
		cerr << "Genetic Algorithm Instance: ERROR: comma selection requires offspringCount >= populationSize" << endl;
	}

	this->commaSelection = commaSelection;
	this->populationSize = populationSize;
	this->offspringCount = offspringCount;
	this->numberOfTurbines = numberOfTurbines;
	this->validityThreshold = validityThreshold;

	long seed = chrono::system_clock::now().time_since_epoch().count();
	randomEngine = default_random_engine(seed);
	cout << "Genetic Algorithm Instance: Using seed " << seed << endl;

	for(long l = 0; l < offspringCount; ++l) {
//		Initialize population with offspringCount individuals. Normally, one would initialize it with the
//		populationSize, but this way we evaluate the same amount of individuals in each iteration (for the sake of
//		consistency). This only affects the first iteration and is thus only minimally wasteful.
		population.push_back(Individual(&wfle, &randomEngine, numberOfTurbines, validityThreshold));
	}
	best.push_back(population[0]);
}

void GA3::run() {
	while(true) {
		evaluatePopulation();
		while(long(population.size()) > populationSize) {
			population.pop_back();
		}
		if(population[0].fitness > best[0].fitness) {
			best[0] = population[0];
		}
		cout << "Fitness after " << wfle.getNumberOfEvaluation() << " evaluations: " << best[0].fitness;
		cout << " (" << best[0].countInvalidTurbines() << " invalid turbines)" << endl;
		vector<long> parentIndices = selectParents();
		vector<Individual> offspring = generateOffspring(parentIndices);

		if(commaSelection) {
			population.clear();
		}
		population.insert(population.end(), offspring.begin(), offspring.end());
	}
}

Matrix<double> GA3::getLayout() {
	return best[0].layout;
}

//	======================================== PRIVATE MEMBERS ========================================

void GA3::evaluatePopulation() {
	for(auto & individual : population) {
		wfle.evaluate(&(individual.layout));
		individual.fitness = wfle.getWakeFreeRatio();
		individual.turbineFitnesses = *(wfle.getTurbineFitnesses());
	}
	sort(population.begin(), population.end());
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
	}
	return parentIndices;
}

vector<Individual> GA3::generateOffspring(vector<long> parentIndices) {
	vector<Individual> offspring;
	for(auto p : parentIndices) {
		Individual i = Individual(population[p]);
		i.mutate();
		offspring.push_back(i);
	}
	return offspring;
}
