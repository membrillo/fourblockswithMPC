
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



