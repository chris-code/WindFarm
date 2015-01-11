#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
#include "eval/Matrix.hpp"
#include "eval/KusiakLayoutEvaluator.h"
#include "GA_3_Individual.hpp"

// TODO
// best should not be a vector
// Stopping criterion
// Continue optimizing after 0 invalid turbines
// Implement optional elitism

class GA3 {
	public:
		GA3(KusiakLayoutEvaluator &evaluator, bool commaSelection, long populationSize, long offspringCount,
		    long numberOfTurbines, double validityThreshold) : wfle(evaluator) {
			if(commaSelection && offspringCount < populationSize) {
				cerr << "Genetic Algorithm Instance: ERROR: comma selection requires offspringCount >= populationSize"
				     << endl;
			}
			cout << "Genetic Algorithm Instance: Population model (" << populationSize;
			if(commaSelection)
				cout << ",";
			else
				cout << "+";
			cout << offspringCount << ")" << endl;

			this->commaSelection = commaSelection;
			this->populationSize = populationSize;
			this->offspringCount = offspringCount;
			this->numberOfTurbines = numberOfTurbines;
			this->validityThreshold = validityThreshold;

			long seed = chrono::system_clock::now().time_since_epoch().count();
			randomEngine = default_random_engine(seed);
			cout << "Genetic Algorithm Instance: Using seed " << seed << endl;

			for(long l = 0; l < populationSize; ++l) {
				population.push_back(Individual(&wfle, &randomEngine, numberOfTurbines, validityThreshold));
			}
			best.push_back(population[0]);
		}

		void run() {
			while(true) {
				evaluatePopulation();
				while(long(population.size()) > populationSize) {
					population.pop_back();
				}
				if(best[0] < population[0]) {  // Not elitism, just remember the best layout across all iterations
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
		Matrix<double> getLayout() {
			return best[0].layout;
		}

	private:
		bool commaSelection;
		long populationSize;
		long offspringCount;
		long numberOfTurbines;
		double validityThreshold;
		KusiakLayoutEvaluator& wfle;
		vector<Individual> population;
		vector<Individual> best; // FIXME this should not be a vector

		default_random_engine randomEngine;

		void evaluatePopulation() {
			for(auto & individual : population) {
				if(individual.fitness >= 0.0) {
//					This individual has already been evaluated (can happen with plus selection)
					continue;
				}
				wfle.evaluate(&(individual.layout));
				individual.fitness = wfle.getWakeFreeRatio();
				individual.turbineFitnesses = *(wfle.getTurbineFitnesses());
			}

			// Mind the REVERSE iterators, sort in descending order (highest fitness first)
			sort(population.rbegin(), population.rend());
		}

		vector<long> selectParents() {
//			TODO tournament selection, maybe?
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

		vector<Individual> generateOffspring(vector<long> parentIndices) {
			vector<Individual> offspring;
			for(auto p : parentIndices) {
				Individual i = Individual(population[p]);
				i.mutate(); // This resets the fitness value
				offspring.push_back(i);
			}
			return offspring;
		}
};
