#include "GA.h"
#include "eval/KusiakLayoutEvaluator.h"

//==================== Helper functions ====================

void printLayout( Matrix<char> &layout ) {
	for( long y=0; y<layout.rows; ++y ) {
		for( long x=0; x<layout.cols; ++x ) {
			if( layout.get( y,x ) == 1 ) {
				cout << "1";
			} else {
				cout << "0";
			}
		}
		cout << endl;
	}
}

//==================== Public members ====================

GA::GA( WindFarmLayoutEvaluator &wfleInput, long numberOfTurbines ):
	wfle( wfleInput ) {
	this->numberOfTurbines = numberOfTurbines;

	grid = generateGrid();
	layout = generateLayout();
}

GA::~GA() {
}

void GA::run() {
	Matrix<double> evalLayout = transformToEvalFormat( layout );
	wfle.evaluate( &evalLayout );
	cout << "Global WakeFreeRatio: " << wfle.getWakeFreeRatio() << endl;

//	Mutate the worst turbine
	while( true ) {
		Matrix<double> *turbineFitnesses = wfle.getTurbineFitnesses();
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
		delete turbineFitnesses;
		cout << "Global WakeFreeRatio after " << wfle.getNumberOfEvaluation() << " evaluations: "
		     << wfle.getWakeFreeRatio() << endl;
	}
}

//==================== Private members ====================

Matrix< vector<double> > GA::generateGrid() {
	KusiakLayoutEvaluator *kusiakPointer = dynamic_cast<KusiakLayoutEvaluator*>( &wfle );

	float buffer = 8.0001 * kusiakPointer->scenario.R;
	long turbinesX = kusiakPointer->scenario.width / buffer;
	long turbinesY = kusiakPointer->scenario.height / buffer;

	Matrix< vector<double> > grid( turbinesY, turbinesX ); //TODO use pair instead
	for( long y=0; y<turbinesY; ++y ) {
		for( long x=0; x<turbinesX; ++x ) {
			vector<double> pos( 2 );
			pos[0] = x * buffer;
			pos[1] = y * buffer;
			grid.set( y, x, pos ); //TODO don't overwrite the vector
		}
	}

	return grid;
}

Matrix< char > GA::generateLayout() {
	//TODO try equally spaced initialization (turbines in obstacles can be placed at the start)
	long placedTurbines = 0;
	Matrix< char > layout( grid.rows, grid.cols );
	for( long y=0; y<grid.rows; ++y ) {
		for( long x=0; x<grid.cols; ++x ) {
			if( isValidPosition( grid.get( y, x )[0], grid.get( y, x )[1] ) &&
			        placedTurbines != numberOfTurbines ) {
				layout.set( y, x, 1 );
				++placedTurbines;
			} else {
				layout.set( y, x, 0 );
			}
		}
	}

	return layout;
}

bool GA::isValidPosition( double posX, double posY ) {
	KusiakLayoutEvaluator *kusiakPointer = dynamic_cast<KusiakLayoutEvaluator*>( &wfle );

	bool valid = true;
	for( long o=0; o<kusiakPointer->scenario.obstacles.rows; ++o ) {
		double minX = kusiakPointer->scenario.obstacles.get( o, 0 );
		double minY = kusiakPointer->scenario.obstacles.get( o, 1 );
		double maxX = kusiakPointer->scenario.obstacles.get( o, 2 );
		double maxY = kusiakPointer->scenario.obstacles.get( o, 3 );
		if( posX >= minX && posX <= maxX && posY > minY && posY <= maxY ) {
			valid = false;
			break;
		}
	}

	return valid;
}

vector< Matrix<char> > GA::mate( long x, long y, long offspringCount ) {
	srand( time( NULL ) ); //FIXME MOVE THIS
	vector< Matrix<char> > newLayouts;
	for( long o = 0; o < offspringCount; ++o ) {
		Matrix<char> newLayout( layout );

		long newX = ( rand() / RAND_MAX ) * ( layout.cols - 1 );
		long newY = ( rand() / RAND_MAX ) * ( layout.rows - 1 );
		while( layout.get( newY, newX ) != 0 ) {
			newX = ( rand() / double( RAND_MAX ) ) * ( layout.cols - 1 );
			newY = ( rand() / double( RAND_MAX ) ) * ( layout.rows - 1 );
		}
		newLayout.set( y, x, 0 );
		newLayout.set( newY, newX, 1 );

		newLayouts.push_back( newLayout );
	}
	return newLayouts;
}

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
