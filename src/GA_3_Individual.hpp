#include <random>
#include "eval/Matrix.hpp"
#include "eval/KusiakLayoutEvaluator.h"

class Individual {
	public:
		Matrix<double> layout;
		double fitness;
		Matrix<double> turbineFitnesses;

		Individual(KusiakLayoutEvaluator *evaluator, default_random_engine *engine, long numberOfTurbines,
		           double validityThreshold):
			wfle(evaluator), randomEngine(engine) {
			this->layout = Matrix<double>(numberOfTurbines, 2);
			this->fitness = -1.0;
			this->turbineFitnesses = Matrix<double>();
			this->validityThreshold = validityThreshold;

			initializeLayoutRandomly();
		}
		
		bool operator<(const Individual &other) const { // Required for sorting
			if(fitness < 0.0) { // This is not evaluated, automatic 'loss'
				return true;
			}
			if(other.fitness < 0.0) { // Other is not evaluated, automatic 'win'
				return false;
			}
			
			long thisInvalid = countInvalidTurbines();
			long otherInvalid = other.countInvalidTurbines();
			if(thisInvalid != otherInvalid) {
				return thisInvalid > otherInvalid;
			}
			
			return fitness < other.fitness;
		}
		
		void mutate() {
			fitness = -1.0;
			
			double sigma = 140.0;
			normal_distribution<double> turbineOffset(0, sigma);
			for(long t = 0; t < layout.rows; ++t) {
				if(turbineFitnesses.get(t, 0) >= validityThreshold)
					continue;

				double newPosX = layout.get(t, 0) + turbineOffset(*randomEngine);
				double newPosY = layout.get(t, 1) + turbineOffset(*randomEngine);
				while(! isValidMutation(t, newPosX, newPosY)) {
					newPosX = layout.get(t, 0) + turbineOffset(*randomEngine);
					newPosY = layout.get(t, 1) + turbineOffset(*randomEngine);
				}
				layout.set(t, 0, newPosX);
				layout.set(t, 1, newPosY);
			}
		}
		
		long countInvalidTurbines() const {
			long invalidTurbines = 0;
			for(long t = 0; t < turbineFitnesses.rows; ++t) {
				if(turbineFitnesses.get(t, 0) <= validityThreshold) {
					++invalidTurbines;
				}
			}
			return invalidTurbines;
		}
		
	private:
		KusiakLayoutEvaluator *wfle;
		default_random_engine *randomEngine;
		double validityThreshold;

		void initializeLayoutRandomly() {
			uniform_real_distribution<double> randomPosX(0, 0 + wfle->scenario.width);
			uniform_real_distribution<double> randomPosY(0, 0 + wfle->scenario.height);
			long placedTurbines = 0;
			while(placedTurbines < layout.rows) {
				double posX = randomPosX(*randomEngine);
				double posY = randomPosY(*randomEngine);

				if(isValidNextTurbine(placedTurbines, posX, posY)) {
					layout.set(placedTurbines, 0, posX);
					layout.set(placedTurbines, 1, posY);
					++placedTurbines;
				}
			}
		}
		
		bool isValidNextTurbine(long placedTurbines, double posX, double posY) {
			if(isOutsideGrid(posX, posY))
				return false;
			if(isInObstacle(posX, posY))
				return false;
			for(long t = 0; t < placedTurbines; ++t)
				if(isTooClose(posX, posY, layout.get(t, 0), layout.get(t, 1)))
					return false;
			return true;
		}
		
		bool isValidMutation(long mutatedTurbineIndex, double newPosX, double newPosY) {
			if(isOutsideGrid(newPosX, newPosY))
				return false;
			if(isInObstacle(newPosX, newPosY))
				return false;
			for(long t = 0; t < layout.rows; ++t) {
				if(t == mutatedTurbineIndex)
					continue;
				if(isTooClose(newPosX, newPosY, layout.get(t, 0), layout.get(t, 1)))
					return false;
			}
			return true;
		}
		
		bool isOutsideGrid(double posX, double posY) {
			if(posX < 0 || posX <= wfle->scenario.width || posY < 0 || posY >= wfle->scenario.height)
				return false;
			return true;
		}
		
		bool isInObstacle(double posX, double posY) {
			for(long o = 0; o < wfle->scenario.obstacles.rows; ++o) {
				double xmin = wfle->scenario.obstacles.get(o, 0);
				double ymin = wfle->scenario.obstacles.get(o, 1);
				double xmax = wfle->scenario.obstacles.get(o, 2);
				double ymax = wfle->scenario.obstacles.get(o, 3);
				if(posX > xmin && posY > ymin && posX < xmax && posY < ymax)
					return true;
			}
			return false;
		}
		
		bool isTooClose(double posX1, double posY1, double posX2, double posY2) {
			double distance = sqrt(pow(posX1 - posX2, 2.0) + pow(posY1 - posY2, 2.0));
			if(distance <= 8 * wfle->scenario.R)
				return true;
			return false;
		}
};
