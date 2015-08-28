#include "gurobi_c++.h"
#include "case9v2.h"
#include "sstream"
#include <stdio.h>

using namespace std;

int Planificacion(int argc, char *argv[]){


	int const rowsLinMat = sizeof linMat / sizeof linMat[0];
	int const rowsBusMat = sizeof BusMat / sizeof BusMat[0];
	int const rowsGenMat = sizeof GenMat / sizeof GenMat[0];
	int const periodos = 5;


	GRBEnv env = GRBEnv();
	GRBModel model = GRBModel(env);

	GRBVar PotenciasGeneradas[rowsGenMat*periodos];

	for (int i = 0; i < rowsGenMat; i++){
		cout << GenMat[1][i] << endl;
	}



	// se actualizan los valores del modelo
	model.update();

	//se optimiza
	model.optimize();

	return 0;


}