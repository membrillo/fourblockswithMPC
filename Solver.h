#pragma once
#include "mpc.h"
//Solver errors
#define ERROR_NO_BUS_MATRIX 0
#define ERROR_NO_GEN_MATRIX 1
#define ERROR_NO_FACTOR_MATRIX 2
#define ERROR_NO_BRANCH_MATRIX 3
#define ERROR_NO_GENCOST_MATRIX 4



class Solver
{
public:
	Solver();
	void solve(mpc mpc);
	~Solver();

private:
	void addVirtualUnits(mpc *mpc);
	void checkValidFile(mpc mpc);
	
};

