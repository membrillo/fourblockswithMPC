
#include "sstream"
#include <stdio.h>
#include "MpcReader.h"
#include "Solver.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Desarrollado por: Felipe Acu�a felipe.acunago@alumnos.usm.cl Carlos Vergara: carlos.vergarab@alumnos.usm.cl             //
// Resolucion de optimizaci�n para un caso de 4 bloques deterministico                                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[]){
	
	MpcReader mpcReader("case4.m");
	mpcReader.read();

	Solver solver;

	solver.solve(mpcReader.getMpc());


}

	


