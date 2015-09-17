#include "Solver.h"
#include "gurobi_c++.h"
#include "jpMatrix.h"
#include "sstream"



Solver::Solver()
{
}

void Solver::addVirtualUnits(mpc *mpc){
	std::vector <int> busNums;
	for (int i = 0; i < mpc->getMatrixVariable("bus").rows; i++){
		if (mpc->getMatrixVariable("bus").getCol(3).at(i)>0){
			busNums.push_back(mpc->getMatrixVariable("bus").getCol(1).at(i));
			std::cout << " Virtual Units added at bus: " << i << std::endl;
		}
	}
	

	std::vector <double> dummyVector;
	for (int i = 0; i < busNums.size(); i++){
		dummyVector.push_back(busNums.at(i)); //1
		dummyVector.push_back(0);// 2
		dummyVector.push_back(0); //3
		dummyVector.push_back(0); //4
		dummyVector.push_back(0); //5
		dummyVector.push_back(0); //6
		dummyVector.push_back(0); //7
		dummyVector.push_back(2); // unidad virtual 8
		dummyVector.push_back(999); //9
		dummyVector.push_back(0); //10
		dummyVector.push_back(0); //11
		dummyVector.push_back(0);// 12
		dummyVector.push_back(0); //13
		dummyVector.push_back(1); //14
		dummyVector.push_back(0); //15
		dummyVector.push_back(0); //16
		dummyVector.push_back(0); //17
		dummyVector.push_back(0);// 18
		dummyVector.push_back(0); //19
		dummyVector.push_back(0); //20
		dummyVector.push_back(0); //21
		dummyVector.push_back(0); //22
		mpc->getMatrixVariable("gen").addRowFromVector(dummyVector);

		dummyVector.clear();
		dummyVector.shrink_to_fit();

		dummyVector.push_back(0); //1
		dummyVector.push_back(0); //2
		dummyVector.push_back(0); //3
		dummyVector.push_back(0); //4
		dummyVector.push_back(0); //5
		dummyVector.push_back(10000); //6
		dummyVector.push_back(0); //7

		mpc->getMatrixVariable("gencost").addRowFromVector(dummyVector);
		dummyVector.clear();
		dummyVector.shrink_to_fit();
	}





}



void Solver::checkValidFile(mpc mpc){
	try{
		if (!mpc.matrixExists("bus")){
			throw ERROR_NO_BUS_MATRIX;
		}
		if (!mpc.matrixExists("gen")){
			throw ERROR_NO_GEN_MATRIX;
		}
		if (!mpc.matrixExists("gencost")){
			throw ERROR_NO_GENCOST_MATRIX;
		}
		if (!mpc.matrixExists("branch")){
			throw ERROR_NO_BRANCH_MATRIX;
		}
		if (!mpc.matrixExists("factor")){
			throw ERROR_NO_FACTOR_MATRIX;
		}
		if (mpc.getMatrixVariable("bus").cols != 13){
			throw "bus";
		}
		if (mpc.getMatrixVariable("gen").cols != 22){
			throw "gen";
		}
		if (mpc.getMatrixVariable("branch").cols != 13){
			throw "branch";
		}
		if (mpc.getMatrixVariable("gencost").cols != 7){
			throw "gencost";
		}


	}
	catch (int ERROR_x){
		switch (ERROR_x){
		case ERROR_NO_BUS_MATRIX:
		{
			std::cout << "FATAL ERROR : BUS matrix was not found on mpc file" << std::endl;
			break;
		}
		case ERROR_NO_GEN_MATRIX:
		{
			std::cout << "FATAL ERROR : GEN matrix was not found on mpc file" << std::endl;
			break;
		}
		case ERROR_NO_FACTOR_MATRIX:
		{
			std::cout << "FATAL ERROR : FACTOR matrix was not found on mpc file" << std::endl;
			break;
		}
		case ERROR_NO_GENCOST_MATRIX:
		{
			std::cout << "FATAL ERROR : GENCOST matrix was not found on mpc file" << std::endl;
			break;
		}
		case ERROR_NO_BRANCH_MATRIX:
		{
			std::cout << "FATAL ERROR : BRANCH matrix was not found on mpc file" << std::endl;
			break;
		}
		}
		abort();
	}
	catch (std::string exception){
		std::cout << "FATAL ERROR: Matrix " << exception << " has bad size";
		abort();
	}

}

void Solver::solve(mpc mpc){


	addVirtualUnits(&mpc);

	checkValidFile(mpc);

	mpc.makePTDF(mpc.getMatrixVariable("branch"), mpc.getMatrixVariable("bus"));
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
				Potencias[i*subperiodos*rowsGenMat + j*rowsGenMat + k].set(GRB_DoubleAttr_UB, mpc.getMatrixVariable("gen").getCol(9).at(k)*mpc.getMatrixVariable("gen").getCol(14).at(k));
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
			n[i*CentralesNoCreadas + j].set(GRB_DoubleAttr_UB, UnidadesMaximas); //Arreglar UB de enes
		}
	}

	model.set(GRB_IntAttr_ModelSense, 1); //Para minimizar la funcion objetivo
	model.update();

	// Restriccion de unidades maximas (sumatoria de n-s es menor o igual a cantidad de unidades maximas)

	for (int i = 0; i < CentralesNoCreadas; i++){
		GRBLinExpr CantidadUnidades = 0;
		ostringstream name;
		for (int j = 0; j < periodos; j++){
			CantidadUnidades += n[j*CentralesNoCreadas + i]; // n11= n[0], n21 = n[1], n31=[2] , n12= n[CentralesNoCreadas], n13= n[CentralesNoCreadas*2]
		}
		name << "Unidades_Maximas_P_" << PosicionesCentralesNoCreadas[i] + 1;
		model.addConstr(CantidadUnidades <= UnidadesMaximas, name.str());
	}

	// Balances de carga

	for (int i = 0; i < periodos; i++){
		for (int j = 0; j < subperiodos; j++){
			GRBLinExpr Generadores = 0;
			for (int k = 0; k < rowsGenMat; k++){
				Generadores += Potencias[i*subperiodos*rowsGenMat + j*rowsGenMat + k];
			}
			ostringstream name;
			name << "Balance_Demanda_" << i + 1 << periodnames[j];
			model.addConstr(Generadores == demandas[j + i*subperiodos], name.str());
		}
	}

	model.update();



	// Criterio de reserva

	for (int i = 0; i < periodos; i++){ // 1 periodo
		for (int l = 0; l < subperiodos; l++){
			if (l == 3)
			{
				ostringstream name; // se inicia el nombre
				GRBLinExpr PotenciasMaximas = 0; // expresion se inicia en 0
				GRBLinExpr enesExp = 0;  // expresion se inicia en 0
				for (int j = 0; j < rowsGenMat; j++){
					if (mpc.getMatrixVariable("gen").getCol(8).at(j) == 1){
						PotenciasMaximas += mpc.getMatrixVariable("gen").getCol(9).at(j); // sumo todos los que no son 0, o sea estan creados
					}
					else if (mpc.getMatrixVariable("gen").getCol(8).at(j) == 0){
						for (int k = -1; k < i; k++){
							enesExp += n[(k + 1)*CentralesNoCreadas + j] * mpc.getMatrixVariable("gen").getCol(9).at(j); // sumo las expresiones de n a un periodo dado
						}
					}
				}
				name << "Criterio_Reserva_" << i + 1 << periodnames[l];
				model.addConstr(PotenciasMaximas + enesExp >= demandas[i*subperiodos + l] * 1.2, name.str());
			}
			else{}
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
			PTDFModified(j, i) = mpc.PTDF(j,filaExtraida - 1);
		}
	}


	matrix demands;
	demands.Init(rowsBusMat, periodos*subperiodos);

	for (int i = 0; i < rowsBusMat; i++){
		for (int j = 0; j < periodos*subperiodos; j++){
			//demands(i, j) = demandaPorPeriodo[i][j];
			demands(i, j) = mpc.getMatrixVariable("Demands").array2D[j][i];
		}
	}

	/*
	matrix PTDFOriginal;
	PTDFOriginal.Init(PTDF, rowsLinMat, rowsBusMat);
	*/

	matrix result;
	




	try{
		//result = PTDFOriginal*demands;
		result = mpc.PTDF*demands;

	}
	catch (jp_error Error){
		cout << Error.msg;
	}

	//result.ToString();
	for (int i = 0; i < periodos; i++){
		for (int j = 0; j < subperiodos; j++){
			for (int l = 0; l < rowsLinMat; l++){
				GRBLinExpr PTDFResultL = 0;
				GRBLinExpr PTDFResultR = 0;
				for (int k = 0; k < rowsGenMat; k++){
					PTDFResultL += PTDFModified(l, k)*Potencias[i*rowsGenMat*subperiodos + k + j*rowsGenMat];
				}
				ostringstream name1;
				name1 << "PTDF_superior_Linea_" << mpc.getMatrixVariable("branch").getCol(1).at(l) << "-" << mpc.getMatrixVariable("branch").getCol(1).at(l) << "_" << i + 1 << periodnames[j];
				PTDFResultR = mpc.getMatrixVariable("branch").getCol(6).at(l) + result(l, i*subperiodos + j);
				model.addConstr(PTDFResultL <= PTDFResultR, name1.str());
				model.update();
				ostringstream name2;
				name2 << "PTDF_inferior_Linea_" << mpc.getMatrixVariable("branch").getCol(2).at(l) << "-" << mpc.getMatrixVariable("branch").getCol(1).at(l) << "_" << i + 1 << periodnames[j];
				PTDFResultR = mpc.getMatrixVariable("branch").getCol(6).at(l) - result(l, i*subperiodos + j);
				model.addConstr(-PTDFResultL <= PTDFResultR, name2.str());
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
					ostringstream name;
					name << "Potencia_Maxima_P_" << i + 1 << periodnames[l];
					model.addConstr(Potencias[i + j*rowsGenMat*subperiodos + l*rowsGenMat] <= PotMax*mpc.getMatrixVariable("gen").getCol(9).at(i), name.str());
					model.update();
				}
				else{


				}
			}
		}
	}



	model.update();
	model.write("archivo.lp");
	std::cout << "############################# OPTIMIZANDO ##################################" << std::endl;

	model.optimize();


	//Para mostrar los resultados


	if (model.get(GRB_IntAttr_SolCount)){
		for (int i = 0; i < periodos*rowsGenMat*subperiodos; i++){

			cout << Potencias[i].get(GRB_StringAttr_VarName) << ": " << Potencias[i].get(GRB_DoubleAttr_X) << endl;

		}
		for (int i = 0; i < periodos; i++){
			for (int j = 0; j < CentralesNoCreadas; j++){
				cout << n[i*CentralesNoCreadas + j].get(GRB_StringAttr_VarName) << ": " << n[i*CentralesNoCreadas + j].get(GRB_DoubleAttr_X) << endl;
			}
		}
	}
	else{
		cout << "No solution found" << endl;
	}


	// se borran las variables de la memoria
	delete[] Potencias;
	delete[] PosicionesCentralesNoCreadas;
	delete[] n;
	delete env;
}

Solver::~Solver()
{
}
