#include "Solver.h"
#include "gurobi_c++.h"
#include "jpMatrix.h"
#include "sstream"
#include "case9v2.h"


Solver::Solver()
{
}

void Solver::solve(mpc mpc){



	mpc.printVariablesStored();



	int const rowsLinMat = mpc.getMatrixVariable("branch").rows;
	int const rowsPTDF = mpc.getMatrixVariable("branch").rows;
	int const rowsBusMat = mpc.getMatrixVariable("bus").rows;
	int const rowsGenMat = mpc.getMatrixVariable("gen").rows;
	int const rowsDemandasSubp = mpc.getMatrixVariable("Demands").rows;
	int const columnsDemandasSubp = mpc.getMatrixVariable("Demands").cols;
	int const periodos = mpc.periods;
	int const subperiodos = mpc.subperiods;
	int const UnidadesMaximas = 3;
	double const interes = 0.1;
	char periodnames[] = { 'v', 'r', 'm', 'p' };


	//generacion matriz de demanda

	//double demandas[columnsDemandasSubp][1];
	double* demandas;
	demandas = new double[columnsDemandasSubp];
	for (int i = 0; i < columnsDemandasSubp; i++){
		double aux = 0;
		for (int k = 0; k < rowsDemandasSubp; k++){
			aux += mpc.getMatrixVariable("Demands").array2D[i][k];
		}
		demandas[i] = aux;
	}

	int CentralesNoCreadas = 0;


	GRBEnv* env = new GRBEnv();

	GRBModel model = GRBModel(*env);

	model.set(GRB_StringAttr_ModelName, "Planificacion");


	for (int i = 0; i < rowsGenMat; i++){
		if (mpc.getMatrixVariable("gen").getCol(8).at(i) == 0){
			CentralesNoCreadas++;
		}
	}

	int* PosicionesCentralesNoCreadas;
	PosicionesCentralesNoCreadas = new int[CentralesNoCreadas];

	int contador = 0;

	for (int i = 0; i < rowsGenMat; i++){
		if (mpc.getMatrixVariable("gen").getCol(8).at(i) == 0){
			PosicionesCentralesNoCreadas[contador] = i;
			//std::cout << "[" << i << "]" << std::endl;
			contador++;
		}
	}



	GRBVar* Potencias;

	Potencias = model.addVars(periodos*rowsGenMat*subperiodos, GRB_CONTINUOUS);

	model.update();
	for (int i = 0; i < periodos; i++){
		for (int j = 0; j < subperiodos; j++){
			for (int k = 0; k < rowsGenMat; k++){
				ostringstream name;
				name << "P" << k + 1 << i + 1 << periodnames[j];

				switch (j){
				case 0:
					Potencias[i*subperiodos*rowsGenMat + j*rowsGenMat + k].set(GRB_DoubleAttr_Obj, 1 / (pow(1 + interes, i)) * 2920 * mpc.getMatrixVariable("gencost").getCol(6).at(k));
					break;
				case 1:
					Potencias[i*subperiodos*rowsGenMat + j*rowsGenMat + k].set(GRB_DoubleAttr_Obj, 1 / (pow(1 + interes, i)) * 1460 * mpc.getMatrixVariable("gencost").getCol(6).at(k));
					break;
				case 2:
					Potencias[i*subperiodos*rowsGenMat + j*rowsGenMat + k].set(GRB_DoubleAttr_Obj, 1 / (pow(1 + interes, i)) * 2555 * mpc.getMatrixVariable("gencost").getCol(6).at(k));
					break;
				case 3:
					Potencias[i*subperiodos*rowsGenMat + j*rowsGenMat + k].set(GRB_DoubleAttr_Obj, 1 / (pow(1 + interes, i)) * 1825 * mpc.getMatrixVariable("gencost").getCol(6).at(k));
					break;
				}
				Potencias[i*subperiodos*rowsGenMat + j*rowsGenMat + k].set(GRB_StringAttr_VarName, name.str()); //P[0]=P11v P[1]=P21v P[2]=p31v P[3]=p41v P[4]=p51v P[5]=p61v P[6]=p71v P[7]=p81v P[8]=p91v P[9]=p11r P[10]=p21r
			}
		}
	}
	model.update();

	GRBVar* n;

	n = model.addVars(periodos*CentralesNoCreadas, GRB_INTEGER);
	model.update();

	double factor = 0;
	double factorFijo = (pow(1 + interes, periodos) - 1) / (interes*pow(1 + interes, periodos - 1));

	for (int i = 0; i < periodos; i++){
		for (int j = 0; j < CentralesNoCreadas; j++){
			ostringstream name;
			name << "n" << j + 1 << i + 1;
			factor = factorFijo - (pow(1 + interes, i) - 1) / (interes*pow(1 + interes, i - 1));
			n[i*CentralesNoCreadas + j].set(GRB_DoubleAttr_Obj, mpc.getMatrixVariable("gen").getCol(9).at(j) * ((mpc.getMatrixVariable("gen").getCol(11).at(j)) / pow(interes + 1, i) + mpc.getMatrixVariable("gen").getCol(13).at(j) * (factor)));
			n[i*CentralesNoCreadas + j].set(GRB_StringAttr_VarName, name.str()); //n21= n del generador 2  (de los no creados) en el periodo 1
		}
	}

	model.set(GRB_IntAttr_ModelSense, 1); //Para minimizar la funcion objetivo
	model.update();

	// Restriccion de unidades maximas (sumatoria de n-s es menor o igual a cantidad de unidades maximas)

	for (int i = 0; i < CentralesNoCreadas; i++){
		GRBLinExpr CantidadUnidades = 0;
		for (int j = 0; j < periodos; j++){
			CantidadUnidades += n[j*CentralesNoCreadas + i]; // n11= n[0], n21 = n[1], n31=[2] , n12= n[CentralesNoCreadas], n13= n[CentralesNoCreadas*2]
		}
		model.addConstr(CantidadUnidades <= UnidadesMaximas, "UnidadesMaximas");

	}

	// Balances de carga
	for (int i = 0; i < periodos; i++){
		for (int j = 0; j < subperiodos; j++){
			GRBLinExpr Generadores = 0;
			for (int k = 0; k < rowsGenMat; k++){
				Generadores += Potencias[i*subperiodos*rowsGenMat + j*rowsGenMat + k];
			}
			model.addConstr(Generadores == demandas[j + i*subperiodos], "PotenciasGeneradas");
		}
	}
	model.update();



	// Criterio de reserva
	for (int i = 0; i < periodos; i++){ // 1 periodo
		for (int l = 0; l < subperiodos; l++){
			ostringstream name; // se inicia el nombre
			GRBLinExpr PotenciasMaximas = 0; // expresion se inicia en 0
			GRBLinExpr enesExp = 0;  // expresion se inicia en 0
			for (int j = 0; j < rowsGenMat; j++){
				if (mpc.getMatrixVariable("gen").getCol(8).at(j) == 1){
					PotenciasMaximas += mpc.getMatrixVariable("gen").getCol(9).at(j); // sumo todos los que no son 0, sea estan creados
				}
				else if (mpc.getMatrixVariable("gen").getCol(8).at(j) == 0){
					for (int k = -1; k < i; k++){
						enesExp += n[(k + 1)*CentralesNoCreadas + j] * mpc.getMatrixVariable("gen").getCol(9).at(j); // sumo las expresiones de n a un periodo dado
					}
				}
			}
			name << "Reserve_critery_periode_" << i + 1 << periodnames[l];
			model.addConstr(PotenciasMaximas + enesExp >= demandas[i*subperiodos + l] * 1.2, name.str());
		}
	}

	model.update();


	// Criterio de PTDF

	// Primero se arregla la matriz


	matrix PTDFModified;
	PTDFModified.Init(rowsPTDF, rowsGenMat);

	for (int i = 0; i < rowsGenMat; i++){
		int filaExtraida = mpc.getMatrixVariable("gen").getCol(1).at(i);
		for (int j = 0; j < rowsPTDF; j++){
			PTDFModified(j, i) = PTDF[j][filaExtraida - 1];
		}
	}


	matrix demands;
	demands.Init(rowsBusMat, periodos);

	for (int i = 0; i < rowsBusMat; i++){
		for (int j = 0; j < periodos; j++){
			//demands(i, j) = demandaPorPeriodo[i][j];
			demands(i, j) = mpc.getMatrixVariable("Demands").array2D[i][j];
		}
	}



	matrix PTDFOriginal;
	PTDFOriginal.Init(PTDF, rowsLinMat, rowsBusMat);


	matrix result;




	try{
		result = PTDFOriginal*demands;

	}
	catch (jp_error Error){
		cout << Error.msg;
	}

	//result.ToString();

	for (int i = 0; i < periodos; i++){
		for (int j = 0; j < rowsLinMat; j++){
			for (int l = 0; l < subperiodos; l++){
				GRBLinExpr PTDFResultL = 0;
				GRBLinExpr PTDFResultR = 0;
				for (int k = 0; k < rowsGenMat; k++){
					PTDFResultL += PTDFModified(j, k)*Potencias[i*rowsGenMat + k + l*rowsGenMat];
				}
				PTDFResultR = mpc.getMatrixVariable("branch").getCol(6).at(j) + result(j, i);
				model.addConstr(PTDFResultL <= PTDFResultR, "PTDF Constrain Superior");
				model.update();
				PTDFResultR = mpc.getMatrixVariable("branch").getCol(6).at(j) - result(j, i);
				model.addConstr(-PTDFResultL <= PTDFResultR, "PTDF Constrain Inferior");
			}

		}

	}


	// Criterio Potencia Maxima

	for (int i = 0; i < rowsGenMat; i++){
		for (int j = 0; j < periodos; j++){
			for (int l = 0; l < subperiodos; l++){
				GRBLinExpr PotMax;
				if (mpc.getMatrixVariable("gen").getCol(8).at(i) == 0){
					for (int k = 0; k < j + 1; k++){
						PotMax += n[k*CentralesNoCreadas + i];
					}
					model.addConstr(Potencias[i + j*rowsGenMat*subperiodos + l*rowsGenMat] <= PotMax*mpc.getMatrixVariable("gen").getCol(9).at(i), "Potencia Maxima");
					model.update();
				}
				else{
					model.addConstr(Potencias[i + j*rowsGenMat*subperiodos + l*rowsGenMat] <= mpc.getMatrixVariable("gen").getCol(9).at(i), "Potencia Maxima");
					model.update();

				}
			}
		}
	}





	model.update();
	model.write("archivo.lp");
	model.optimize();

	//Para mostrar los resultados
	for (int i = 0; i < periodos; i++){
		for (int j = 0; j < rowsGenMat; j++){
			cout << Potencias[i*rowsGenMat + j].get(GRB_StringAttr_VarName) << ": " << Potencias[i*rowsGenMat + j].get(GRB_DoubleAttr_X) << endl;
		}
	}
	for (int i = 0; i < periodos; i++){
		for (int j = 0; j < CentralesNoCreadas; j++){
			cout << n[i*CentralesNoCreadas + j].get(GRB_StringAttr_VarName) << ": " << n[i*CentralesNoCreadas + j].get(GRB_DoubleAttr_X) << endl;
		}
	}




	// se borran las variables de la memoria
	delete[] Potencias;
	delete[] PosicionesCentralesNoCreadas;

	delete env;


}

Solver::~Solver()
{
}
