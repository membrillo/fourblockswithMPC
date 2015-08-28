#pragma once
#include <string>
#include "mpcMatrix.h" 
#include <vector>

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

	void createDemandsMatrix();


	void addVariable(std::string newName, double value);
	void clearMemory();
	double& getVariable(std::string nameToSearch);
	~mpc();

};





