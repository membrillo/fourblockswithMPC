#include "jpError.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
//typedef unsigned int UINT;
using namespace std;


class matrix{
	double **arr;
	bool ninit = 0;
public:
	UINT nrows, ncols;
	matrix();
	matrix(UINT rowss, UINT colss);
	bool IsInit();
	double& operator()(const UINT n, const UINT m);
	void operator=(matrix &oth_mat);


	//ARREGLAR ESTA FUNCION Y CREAR UNA FUNCION DE PRINTEO BUENA:
	void ToString(){
		cout << "------" << endl;
		for (size_t i = 0; i < nrows; ++i){
			for (size_t j = 0; j < ncols; ++j){
				cout << arr[i][j] << "  ";
			}
			cout << endl;
		}
		cout << "------" << endl;
	}

	void Init(UINT rowss, UINT colss);
	template <size_t size_x, size_t size_y> void Init(double(&ar)[size_x][size_y], UINT rowss, UINT colss);
	matrix t();
	matrix  operator*(matrix &from_2);
	~matrix();

};

//EXTERNAL FUNCTIONS OVER MATRIXES:



//TEMPLATES DEFINITIONS:

template <size_t size_x, size_t size_y> void matrix::Init(double(&ar)[size_x][size_y], UINT rowss, UINT colss){
	nrows = rowss;
	ncols = colss;
	arr = new double*[nrows];
	for (UINT i = 0; i < nrows; ++i){
		arr[i] = new double[ncols];
	}
	for (UINT i = 0; i < rowss; ++i){
		for (int j = 0; j < colss; ++j){
			arr[i][j] = ar[i][j];
		}
	}
	ninit = 1;
}
