#pragma once
#include "mpc.h"
class Solver
{
public:
	Solver();
	void solve(mpc mpc);
	~Solver();
private:
	void addVirtualUnits(mpc *mpc);
};

