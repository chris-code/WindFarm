#include "GA.h"
#include "eval/KusiakLayoutEvaluator.h"
#include <algorithm>

//==================== Helper functions ====================

void printLayout(Matrix<char> &layout) {
	for (long y = 0; y < layout.rows; ++y) {
		for (long x = 0; x < layout.cols; ++x) {
			if (layout.get(y, x) == 1) {
				cout << "1";
			} else {
				cout << "0";
			}
		}
		cout << endl;
	}
}

//==================== Public members ====================

GA::GA(WindFarmLayoutEvaluator &wfleInput, long numberOfTurbines) :
		wfle(wfleInput) {
	this->numberOfTurbines = numberOfTurbines;

	grid = generateGrid();
	layout = generateLayout();
}

GA::~GA() {
}

void GA::run() {
	while( true ) {
		Matrix<double> evalLayout = transformToEvalFormat( layout );
		wfle.evaluate( &evalLayout );
		cout << "Global WakeFreeRatio: " << wfle.getWakeFreeRatio() << endl;

		/*Matrix<double> *turbineFitnesses = wfle.getTurbineFitnesses();
		double worstValue = turbineFitnesses->get( 0, 0 );
		long worstXindex = evalLayout.get( 0, 2 );
		long worstYindex = evalLayout.get( 0, 3 );
		for( long t=1; t<turbineFitnesses->rows; ++t ) {
			if( turbineFitnesses->get( t, 0 ) < worstValue ) {
				worstValue = turbineFitnesses->get( t, 0 );
				worstXindex = evalLayout.get( t, 2 );
				worstYindex = evalLayout.get( t, 3 );
			}
		}

		double bestFitness = 0;
		long bestLayout = -1;
		vector< Matrix<char> > newLayouts = mate( worstXindex, worstYindex, 1 );
		for( long l=0; l<long( newLayouts.size() ); ++l ) {
			evalLayout = transformToEvalFormat( newLayouts[l] );
			wfle.evaluate( &evalLayout );
			if( wfle.getWakeFreeRatio() > bestFitness ) {
				bestFitness = wfle.getWakeFreeRatio();
				bestLayout = l;
			}
		}
		layout = newLayouts[bestLayout];
		delete turbineFitnesses;*/
		
		//long offspringCount = 10;
		//vector< pair<long, long> > parents = selectParents( evalLayout , 10 );
		//vector< Matrix<char> > newLayouts = mate( parents, offspringCount );
		cout << "Global WakeFreeRatio after " << wfle.getNumberOfEvaluation() << " evaluations: "
		     << wfle.getWakeFreeRatio() << endl;
	}
}

//==================== Private members ====================

Matrix<vector<double> > GA::generateGrid() {
	KusiakLayoutEvaluator *kusiakPointer =
			dynamic_cast<KusiakLayoutEvaluator*>(&wfle);

	float buffer = 8.0001 * kusiakPointer->scenario.R;
	long turbinesX = kusiakPointer->scenario.width / buffer;
	long turbinesY = kusiakPointer->scenario.height / buffer;

	Matrix<vector<double> > grid(turbinesY, turbinesX); //TODO use pair instead
	for (long y = 0; y < turbinesY; ++y) {
		for (long x = 0; x < turbinesX; ++x) {
			vector<double> pos(2);
			pos[0] = x * buffer;
			pos[1] = y * buffer;
			grid.set(y, x, pos); //TODO don't overwrite the vector
		}
	}

	return grid;
}

Matrix<char> GA::generateLayout() {
	//TODO try equally spaced initialization (turbines in obstacles can be placed at the start)
	long placedTurbines = 0;
	Matrix<char> layout(grid.rows, grid.cols);
	for (long y = 0; y < grid.rows; ++y) {
		for (long x = 0; x < grid.cols; ++x) {
			if (isValidPosition(grid.get(y, x)[0], grid.get(y, x)[1])
					&& placedTurbines != numberOfTurbines) {
				layout.set(y, x, 1);
				++placedTurbines;
			} else {
				layout.set(y, x, 0);
			}
		}
	}

	return layout;
}

bool GA::isValidPosition(double posX, double posY) {
	KusiakLayoutEvaluator *kusiakPointer =
			dynamic_cast<KusiakLayoutEvaluator*>(&wfle);

	bool valid = true;
	for (long o = 0; o < kusiakPointer->scenario.obstacles.rows; ++o) {
		double minX = kusiakPointer->scenario.obstacles.get(o, 0);
		double minY = kusiakPointer->scenario.obstacles.get(o, 1);
		double maxX = kusiakPointer->scenario.obstacles.get(o, 2);
		double maxY = kusiakPointer->scenario.obstacles.get(o, 3);
		if (posX >= minX && posX <= maxX && posY > minY && posY <= maxY) {
			valid = false;
			break;
		}
	}

	return valid;
}

// this method is a comparator for (fitness value, index)-pairs
bool compareFitnesses(pair<double, long> fit1, pair<double, long> fit2) {
	// return true if the first pair is smaller than the second one
	return fit1.first < fit2.first;
}


vector<pair<long, long> > GA::selectParents(Matrix<double> &evalLayout, long howMany) {
	if (howMany > long(evalLayout.rows))
	{
		cerr << "You can't choose more parents than turbines exist." << endl;
		exit(-1);
	}

	Matrix<double> *turbineFitnesses = wfle.getTurbineFitnesses();

	vector<pair<double, long> > fitnessIndexPairs;

	for (long i = 0; i < (long) turbineFitnesses->rows; i++)
		fitnessIndexPairs.push_back(make_pair(turbineFitnesses->get(i, 0), i));

	sort(fitnessIndexPairs.begin(), fitnessIndexPairs.end(), compareFitnesses);

	vector<pair<long, long> > coordinates;

	for (long i = 0; i < howMany; i++) {

		double sumOfRanks = 0.5 * double(fitnessIndexPairs.size()) * (double(fitnessIndexPairs.size()) + 1);

		double random = (double(rand()) / RAND_MAX) * sumOfRanks;

		for (long i = 1; i <= long(fitnessIndexPairs.size()); i++) {
			if (random < (fitnessIndexPairs.size()) - i) {
				long x = evalLayout.get(fitnessIndexPairs[i-1].second, 2);
				long y = evalLayout.get(fitnessIndexPairs[i-1].second, 3);

				coordinates.push_back(make_pair(x,y));

				fitnessIndexPairs.erase(fitnessIndexPairs.begin() + i - 1);

				cout << "Turbine chosen as parent at (" << x << "," << y <<") with fitness" << fitnessIndexPairs[i-1].first << endl;

				break;
			}
			random -= fitnessIndexPairs.size() - i;
		}
	}

	delete turbineFitnesses;

	return coordinates;
}


// Chrisbot-is-not-content version:
//vector<pair<long, long> > GA::selectParents(Matrix<double>& evalLayout,
//		long howMany) {
//	if (howMany > long(evalLayout.rows))
//	{
//		cerr << "You can't choose more parents than turbines exist." << endl;
//		exit(-1);
//	}
//
//	Matrix<double> *turbineFitnesses = wfle.getTurbineFitnesses();
//
//	vector<pair<double, long> > fitnessIndexPairs;
//
//	for (long i = 0; i < (long) turbineFitnesses->rows; i++)
//		fitnessIndexPairs.push_back(make_pair(turbineFitnesses->get(i, 0), i));
//
//	sort(fitnessIndexPairs.begin(), fitnessIndexPairs.end(), compareFitnesses);
//
//	vector<pair<long, long> > coordinates;
//
//	long index = 0;
//
//	while (long(coordinates.size()) < howMany) {
//
//		long x = evalLayout.get(fitnessIndexPairs[index].second, 2);
//		long y = evalLayout.get(fitnessIndexPairs[index].second, 3);
//
//		bool alreadyChosen = false;
//		for (long i = 0; i < long(coordinates.size()); i++) {
//			if (coordinates[i].first == x && coordinates[i].second == y)
//				alreadyChosen = true;
//		}
//		if (alreadyChosen == true)
//			continue;
//
//		double probability = 1 - (index / double(fitnessIndexPairs.size()));
//
//		if ((rand() / RAND_MAX) < probability)
//			coordinates.push_back(make_pair(x, y));
//
//		index = (index + 1) % coordinates.size();
//	}
//
//	delete turbineFitnesses;
//
//	return coordinates;
//}

vector<Matrix<char> > GA::mate(long x, long y, long offspringCount) {
	srand(time(NULL)); //FIXME MOVE THIS
	vector<Matrix<char> > newLayouts;
	for (long o = 0; o < offspringCount; ++o) {
		Matrix<char> newLayout(layout);

		long newX = (rand() / RAND_MAX) * (layout.cols - 1);
		long newY = (rand() / RAND_MAX) * (layout.rows - 1);
		while (layout.get(newY, newX) != 0) {
			newX = (rand() / double(RAND_MAX)) * (layout.cols - 1);
			newY = (rand() / double(RAND_MAX)) * (layout.rows - 1);
		}
		newLayout.set(y, x, 0);
		newLayout.set(newY, newX, 1);

		newLayouts.push_back(newLayout);
	}
	return newLayouts;
}

/*vector< Matrix<char> > mate( vector< pair<long, long> > turbines, long offspringCount ) {
	vector< Matrix<char> > offspringPopulation;
	for( long o=0; o<offspringCount; ++o) {
		Matrix<char> offspring(layout);
		offspringPopulation.push_back(offspring);
	}
	
	return offspringPopulation;
}*/

//	TODO describe the format of the returned matrix
Matrix<double> GA::transformToEvalFormat( Matrix<char> &layoutToTransform ) {
	long transformedTurbines = 0;
	Matrix<double> evalLayout( numberOfTurbines, 4 );
	for( long y=0; y<grid.rows; ++y ) {
		for( long x=0; x<grid.cols; ++x ) {
			if( layoutToTransform.get( y, x ) == 1 ) {
				evalLayout.set( transformedTurbines, 0, grid.get( y, x )[0] );
				evalLayout.set( transformedTurbines, 1, grid.get( y, x )[1] );
				evalLayout.set( transformedTurbines, 2, x );
				evalLayout.set( transformedTurbines, 3, y );
				++transformedTurbines;
			}
		}
	}
	return evalLayout;
}
