#pragma once
#include <string>
#include "mpcMatrix.h" 
#include "jpMatrix.h"
#include <vector>
#include "linalg.h"


struct variable{
	std::string name;
public: double value;
};

class mpc
{
public:
	mpc(int busRows,int busCols, int genRows, int genCols, int branchRows, int branchCols);
	mpc();
	std::vector <mpcMatrix> matrixes;
	std::vector <variable> variables;
	void addNewMatrix(std::string name, int rows, int cols);
	void addNewMatrix(std::string name);
	mpcMatrix& getMatrixVariable(std::string name);
	bool matrixExists(std::string name);
	void printVariableNames();
	void writeMatrix(mpcMatrix matrixToWrite);
	void writeMatrix(std::string name);
	void writeAllMatrix();
	void printVariablesStored();
	int periods;
	int subperiods;
	matrix PTDF;
	void createDemandsMatrix();
	void addVariable(std::string newName, double value);
	void clearMemory();
	double& getVariable(std::string nameToSearch);
	void makePTDF(mpcMatrix branch, mpcMatrix bus);
	~mpc();

private:
	matrix zeros(int rowss, int colss);
	void inverse(matrix &from_1, matrix &ret);
	matrix diag(matrix &a);
	matrix mpcMatrixEquivalent(mpcMatrix EqMatrix);
	void jpMakePTDF(matrix &branch, matrix &ret, int nb);
	

};





