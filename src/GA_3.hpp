#include <algorithm>
#include <deque>
#include <chrono>
#include <random>
#include "eval/Matrix.hpp"
#include "eval/KusiakLayoutEvaluator.h"
#include "GA_3_Individual.hpp"

// TODO
// Continue optimizing after 0 invalid turbines

class GA3 {
	public:
		GA3(KusiakLayoutEvaluator &evaluator, bool commaSelection, long populationSize, long offspringCount,
		    bool elitism, long numberOfTurbines, double validityThreshold) :
			wfle(evaluator),
			randomEngine(chrono::system_clock::now().time_since_epoch().count()),
			best(&wfle, &randomEngine, numberOfTurbines, validityThreshold) {
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
			this->elitism = elitism;
			this->numberOfTurbines = numberOfTurbines;
			this->validityThreshold = validityThreshold;

			for(long l = 0; l < populationSize; ++l) {
				population.push_back(Individual(&wfle, &randomEngine, numberOfTurbines, validityThreshold));
			}
			best = population.front();
		}

		void run() {
			while(wfle.getNumberOfEvaluation() + offspringCount <= 1000) {
//				Evaluate population and perform environmental selection
				evaluatePopulation();
				while(long(population.size()) > populationSize) {
					population.pop_back();
				}
				if(best < population.front()) {  // Not elitism, just remember best layout across all iterations
					best = population.front();
				}
//				if((wfle.getNumberOfEvaluation() - populationSize) % (50 * offspringCount) == 0) {
				cout << "Fitness after " << wfle.getNumberOfEvaluation() << " evaluations: " << best.fitness;
				cout << " (" << best.countInvalidTurbines() << " invalid turbines)" << endl;
//				}

//				Select parents and create offspring
				deque<long> parentIndices = selectParents();
				deque<Individual> offspring = generateOffspring(parentIndices);

//				Build new population
				if(commaSelection) {
					population.clear();
				}
				population.insert(population.end(), offspring.begin(), offspring.end());
				if(elitism) {
					population.push_front(best);
				}
			}
			cout << "Fitness after " << wfle.getNumberOfEvaluation() << " evaluations: " << best.fitness;
			cout << " (" << best.countInvalidTurbines() << " invalid turbines)" << endl;
		}
		Matrix<double> getLayout() {
			return best.layout;
		}

	private:
		long numberOfTurbines;
		double validityThreshold;
		KusiakLayoutEvaluator& wfle;

		default_random_engine randomEngine;
		bool commaSelection;
		long populationSize;
		long offspringCount;
		bool elitism;
		deque<Individual> population;
		Individual best;

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

		deque<long> selectParents() {
//			TODO tournament selection, maybe?
			uniform_int_distribution<long> selector(1, (populationSize * (populationSize + 1)) / 2);
			deque<long> parentIndices;
			for(long p = 0; p < offspringCount; ++p) {
				long randomValue = selector(randomEngine); //FIXME
				long sum = 0;
				for(long l = 0; l < populationSize; ++l) {
					sum += populationSize - l;
					if(randomValue <= sum) {
						parentIndices.push_back(l);
						break;
					}
				}
			}
			return parentIndices;
		}

		deque<Individual> generateOffspring(const deque<long> &parentIndices) {
			deque<Individual> offspring;
			for(auto p : parentIndices) {
				Individual i = Individual(population.at(p));
				i.mutate(); // This resets the fitness value
				offspring.push_back(i);
			}
			return offspring;
		}
};
