#include "GA.h"
#include "eval/KusiakLayoutEvaluator.h"

GA::GA( WindFarmLayoutEvaluator& wfleInput ):
	wfle( wfleInput ){
	numberOfLayouts = 20;
	
	grid = generateGrid();
}

GA::~GA() {
}

void GA::run() {
	double v = wfle.evaluate(&grid);
	cout << "Value = " << v << endl;
}

//////////////////// Private members ////////////////////

Matrix<double> GA::generateGrid() {
	KusiakLayoutEvaluator *kusiakPointer = dynamic_cast<KusiakLayoutEvaluator*>(&wfle);
	
	cout << "Size of area: " << kusiakPointer->scenario.width << "x" << kusiakPointer->scenario.height << endl;
	for(long o=0; o<kusiakPointer->scenario.obstacles.rows; ++o) {
		double minX = kusiakPointer->scenario.obstacles.get(o, 0);
		double minY = kusiakPointer->scenario.obstacles.get(o, 1);
		double maxX = kusiakPointer->scenario.obstacles.get(o, 2);
		double maxY = kusiakPointer->scenario.obstacles.get(o, 3);
		cout << "(" << minX << "," << minY << ")" << endl;
		cout << "(" << maxX << "," << maxY << ")" << endl << endl;
	}
	
	double buffer = 8.0001 * kusiakPointer->scenario.R;
	long turbinesX = kusiakPointer->scenario.width / buffer;
	long turbinesY = kusiakPointer->scenario.height / buffer;
	
	vector< pair<double, double> > temporaryGrid;
	for(long y=0; y<=turbinesY; ++y) {
		double posY = y * buffer;
		for(long x=0; x<=turbinesX; ++x) {
			double posX = x * buffer;
			if( isValidPosition(posX, posY) ) {
				temporaryGrid.push_back(make_pair(posX, posY));
				cout << "1";
			}else{cout << "0";}
		}
		cout << endl;
	}
	
	Matrix<double> grid = Matrix<double>(temporaryGrid.size(), 2);
	for(long t=0; t<long(temporaryGrid.size()); ++t) {
		grid.set(t, 0, temporaryGrid[t].first);
		grid.set(t, 1, temporaryGrid[t].second);
	}
	grid.set(0, 0, 3500);
	grid.set(0, 1, 4500);
	cout << "Turbine count: " << grid.rows << endl;
	return grid;
}

bool GA::isValidPosition(double posX, double posY) {
	KusiakLayoutEvaluator *kusiakPointer = dynamic_cast<KusiakLayoutEvaluator*>(&wfle);
	
	bool valid = true;
	for(long o=0; o<kusiakPointer->scenario.obstacles.rows; ++o) {
		double minX = kusiakPointer->scenario.obstacles.get(o, 0);
		double minY = kusiakPointer->scenario.obstacles.get(o, 1);
		double maxX = kusiakPointer->scenario.obstacles.get(o, 2);
		double maxY = kusiakPointer->scenario.obstacles.get(o, 3);
		if(posX >= minX && posX <= maxX && posY > minY && posY <= maxY) {
			valid = false;
			break;
		}
	}
	
	return valid;
	//return true;
}