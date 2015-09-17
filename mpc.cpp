
#include "mpc.h"
#include <iostream>
#include <fstream>


mpc::mpc(int busRows, int busCols, int genRows, int genCols, int branchRows, int branchCols)
{
	
}

mpc::mpc(){
	
}

void mpc::createDemandsMatrix(){
	if (matrixExists("factor")){
		addNewMatrix("Demands");
		periods = getMatrixVariable("factor").rows;
		subperiods = getMatrixVariable("factor").cols;

		for (int i = 0; i < getMatrixVariable("factor").rows; i++){
			for (int j = 0; j < getMatrixVariable("factor").cols; j++){
				getMatrixVariable("Demands").addColumnFromVector(getMatrixVariable("bus").getCol(3), getMatrixVariable("factor").array2D[j][i]);
			}
		}
	}
}

void mpc::writeMatrix(mpcMatrix matrixToWrite){
	std::ofstream myfile;
	std::string extension = ".jpmatrix";
	std::string fileToWrite = matrixToWrite.name + "Matrix" + extension;
	myfile.open(fileToWrite);
	myfile << matrixToWrite.rows << "," << matrixToWrite.cols << " \n";
	for (int y = 0; y < matrixToWrite.rows; y++){
		myfile << matrixToWrite.array2D[0][y];
		for (int x = 1; x < matrixToWrite.cols; x++){
			myfile << ","<< matrixToWrite.array2D[x][y];
		}
		myfile << " \n";
	}

	myfile.close();
}

void mpc::writeMatrix(std::string name){
	mpcMatrix matrixToWrite;
	matrixToWrite = getMatrixVariable(name);
	std::ofstream myfile;
	std::string extension = ".jpmatrix";
	std::string fileToWrite = name + "Matrix" + extension;
	myfile.open(fileToWrite);
	myfile << matrixToWrite.rows << "," << matrixToWrite.cols << " \n";
	for (int y = 0; y < matrixToWrite.rows; y++){
		myfile << matrixToWrite.array2D[0][y];
		for (int x = 1; x < matrixToWrite.cols; x++){
			myfile << "," << matrixToWrite.array2D[x][y];
		}
		myfile << " \n";
	}

	myfile.close();
}


void mpc::addNewMatrix(std::string name, int cols, int rows){
	if (matrixExists(name)){
		getMatrixVariable(name).clearMatrix();
		getMatrixVariable(name).resize(rows, cols);
	}
	else{
		mpcMatrix newMpcMatrix(name, rows, cols);
		matrixes.push_back(newMpcMatrix);
	}
}



void mpc::addNewMatrix(std::string name){
	mpcMatrix newMpcMatrix(name);
	matrixes.push_back(newMpcMatrix);
}

void mpc::printVariablesStored(){

	std::cout << "Numero de Variables : " << variables.size() << " // Numero de matrices : " << matrixes.size() << std::endl;
	std::cout << "####### VARIABLES STORED ###################################" << std::endl;
	for (int i = 0; i<variables.size(); i++){
		std::cout << variables.at(i).name << " : " << getVariable(variables.at(i).name) << std::endl;
	}

	std::cout << "####### MATRIXES STORED ###################################" << std::endl;

	for (int i = 0; i<matrixes.size(); i++){
		matrixes.at(i).printMatrix();
	}

	std::cout << "####### PTDF GENERATED ###################################" << std::endl;
	PTDF.ToString();


}

void mpc::writeAllMatrix(){
	for (int i = 0; i < matrixes.size(); i++){
		writeMatrix(matrixes.at(i));
	}
}

mpcMatrix& mpc::getMatrixVariable(std::string name){
	int index = 0;
	for (int i = 0; i < matrixes.size(); i++){
		if (matrixes.at(i).name.compare(name) == 0){
			index = i;
			break;
		}
	}
	return matrixes.at(index);
}

bool mpc::matrixExists(std::string name){
	bool exists = false;

	for (int i = 0; i < matrixes.size(); i++){
		if (matrixes.at(i).name.compare(name) == 0){
			exists = true;
			break;
		}
	}
	return exists;

}

void mpc::printVariableNames(){
	
}

void mpc::clearMemory(){
	matrixes.clear();
	matrixes.shrink_to_fit();

	variables.clear();
	variables.shrink_to_fit();
}

double& mpc::getVariable(std::string name){
	int index=0;
	for (int i = 0; i < variables.size(); i++){
		if (variables.at(i).name.compare(name)==0){
			index = i;
			break;
		}
		}
	return variables.at(index).value;
}

void mpc::addVariable(std::string newName, double valueToSet){
	variable newVar;
	newVar.name = newName;
	variables.push_back(newVar);
	//std::cout << valueToSet << std::endl;
	getVariable(newName) = valueToSet;

}



mpc::~mpc()
{
}

matrix mpc::zeros(int rowss, int colss){
	matrix *ret = new matrix(rowss, colss);
	for (size_t i = 0; i < rowss; ++i){
		for (size_t j = 0; j < colss; ++j){
			(*ret)(i, j) = 0;
		}
	}
	return (*ret);
}

matrix mpc::mpcMatrixEquivalent(mpcMatrix EqMatrix){
	int rowss = EqMatrix.rows;
	int colss = EqMatrix.cols;
	matrix *ret = new matrix(rowss, colss);
	for (size_t i = 0; i < rowss; ++i){
		for (size_t j = 0; j < colss; ++j){
			(*ret)(i, j) = EqMatrix.array2D[j][i];
		}
	}
	return (*ret);
}

void mpc::inverse(matrix &from_1, matrix &ret){
	int n;
	//matrix *ret = new matrix(from_1.nrows, from_1.ncols);
	ret.Init(from_1.nrows, from_1.ncols);
	alglib::ae_int_t info;
	alglib::matinvreport rep;
	alglib::real_2d_array a, b;
	if (from_1.nrows == from_1.ncols){
		try{
			n = from_1.nrows;
			a.setlength(n, n);
			for (size_t i = 0; i < n; ++i){
				for (size_t j = 0; j < n; ++j){
					a[i][j] = from_1(i, j);

				}
			}

			alglib::rmatrixinverse(a, info, rep);
			if (int(info) != 1) throw jp_error(INVERSION_INFO_ERROR);
			//cout << endl << "matrix inversion info: " << int(info) << endl;
			//cout << a.tostring(2).c_str() << endl;
			for (size_t i = 0; i < n; ++i){
				for (size_t j = 0; j < n; ++j){
					ret(i, j) = a[i][j];
				}
			}
			//ret.ToString();
			return;
		}
		catch (alglib::ap_error error){
			//cout << error.msg << endl;
		}
	}
	else{
		throw jp_error(NOT_SQUARE_MATRIX);
	}
	int rows, cols;
}

matrix mpc::diag(matrix &a){
	matrix *ret = new matrix;

	if (a.ncols == 1){
		//ret.Init(a.nrows, a.nrows);
		*ret = zeros(a.nrows, a.nrows);
		for (size_t i = 0; i < a.nrows; i++){
			(*ret)(i, i) = a(i, 0);
		}
	}
	else if (a.nrows == 1){
		*ret = zeros(a.ncols, a.ncols);
		for (size_t i = 0; i < a.ncols; i++){
			(*ret)(i, i) = a(0, i);
		}
	}
	else{
		throw jp_error(DIAG_NOT_ONE_DIMENSION);
	}
	return *ret;
}

void mpc::jpMakePTDF(matrix &branch, matrix &ret, int nb){
	matrix A, B, Ybus;
	matrix aux_1, aux_2, aux_3, aux_4, aux_5, aux_6, aux_t;
	UINT nl, count;
	nl = branch.nrows;
	A = zeros(nl, nb);
	count = 0;

	
	while (count < nl){
		A(count, branch(count, 0) - 1) = 1;
		A(count, branch(count, 1) - 1) = -1;
		count = count + 1;
	}
	
	aux_1.Init(nl, 1);
	for (size_t i = 0; i < nl; i++){
		aux_1(i, 0) = 1 / branch(i, 3);
	}
	B = diag(aux_1);
	aux_2 = B*A;
	aux_t = A.t();
	aux_3 = aux_t*aux_2;
	aux_4.Init(nb - 1, nb - 1);
	for (size_t i = 0; i < nb - 1; i++){
		for (size_t j = 0; j < nb - 1; j++){
			aux_4(i, j) = aux_3(i + 1, j + 1);
		}
	}
	inverse(aux_4, aux_6);

	for (size_t i = 1; i < nb; i++){
		for (size_t j = 1; j < nb; j++){
			aux_3(i, j) = aux_6(i - 1, j - 1);
		}
	}
	for (size_t i = 0; i < nb; i++){
		aux_3(i, 0) = 0;
		aux_3(0, i) = 0;
	}
	aux_5 = A*aux_3;
	ret = B*aux_5;
}

void mpc::makePTDF(mpcMatrix branch, mpcMatrix bus){
	matrix fixedMatrix;

	int rows = branch.rows;
	int cols = bus.rows;
	mpcMatrix auxMatrix;

	auxMatrix.addColumnFromVector(branch.getCol(1),1);
	auxMatrix.addColumnFromVector(branch.getCol(2),1);
	auxMatrix.addColumnFromVector(branch.getCol(3),1);
	auxMatrix.addColumnFromVector(branch.getCol(4),1);
	auxMatrix.addColumnFromVector(branch.getCol(6),1);

	
	fixedMatrix = mpcMatrixEquivalent(auxMatrix);
	
	jpMakePTDF(fixedMatrix, PTDF, cols);
}

