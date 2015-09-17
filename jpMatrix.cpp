#include "jpMatrix.h"

matrix::matrix(){
	ninit = 0;
};
matrix::matrix(UINT rowss, UINT colss){
	nrows = rowss;
	ncols = colss;
	arr = new double*[rowss];
	for (int i = 0; i < rowss; ++i){
		arr[i] = new double[colss];
	}
	ninit = 1;
};
void matrix::Init(UINT rowss, UINT colss){
	if (ninit == 0){
		nrows = rowss;
		ncols = colss;
		arr = new double*[nrows];
		for (int i = 0; i < nrows; ++i){
			arr[i] = new double[ncols];
		}
	}
	else{
		throw jp_error(MATRIX_IS_ALREADY_INIT);
	}

};
void matrix::Init(double **arr, UINT rowss, UINT colss){
	nrows = rowss;
	ncols = colss;
	arr = new double*[nrows];
	for (UINT i = 0; i < nrows; ++i){
		arr[i] = new double[ncols];
	}
	for (UINT i = 0; i < rowss; ++i){
		for (int j = 0; j < colss; ++j){
			arr[i][j] = arr[i][j];
		}
	}
	ninit = 1;

};
double& matrix::operator()(const UINT n, const UINT m){
	if (n >= nrows || m >= ncols){
		throw jp_error(OUT_OF_INDEX);
	}
	else{ return arr[n][m]; }

};

void matrix::operator=(matrix &oth_mat){
	if (oth_mat.ncols == this->ncols && oth_mat.nrows == this->nrows){
		for (int i = 0; i < oth_mat.nrows; ++i){
			for (int j = 0; j < oth_mat.ncols; ++j){
				arr[i][j] = oth_mat(i, j);
			}
		}
	}
	else if (!ninit){
		nrows = oth_mat.nrows;
		ncols = oth_mat.ncols;
		arr = new double*[nrows];
		for (int i = 0; i < nrows; ++i){
			arr[i] = new double[ncols];
		}
		for (int i = 0; i < oth_mat.nrows; ++i){
			for (int j = 0; j < oth_mat.ncols; ++j){
				arr[i][j] = oth_mat(i, j);
			}
		}
		ninit = 1;
	}
	else{
		throw jp_error(NOT_EQUAL);
	}

}


matrix matrix::t(){
	matrix *ret = new matrix(this->ncols, this->nrows);
	for (int i = 0; i < this->nrows; ++i){
		for (int j = 0; j < this->ncols; ++j){
			(*ret)(j, i) = arr[i][j];
		}
	}
	return *ret;
}



matrix matrix::operator*(matrix &from_2){
	UINT n = this->nrows;
	UINT p = from_2.ncols;
	UINT m = from_2.nrows;
	matrix *ret = new matrix(n, p);
	//matrix aux = *ret;
	if (this->ncols == m){
		double sumaux;
		for (UINT i = 0; i < n; i++){
			for (UINT j = 0; j < p; j++){
				sumaux = 0;
				for (UINT k = 0; k < m; k++){
					sumaux = sumaux + arr[i][k] * from_2(k, j);
				}
				(*ret)(i, j) = sumaux;
			}
		}
		return *ret;
	}
	else{
		throw jp_error(DOT_COL_ROW);
	}

}

matrix::~matrix(){
	if (this->arr != NULL){
		for (int i = 0; i < nrows; ++i){
			delete[] arr[i];
		}
		delete arr;
	}
#ifdef verbose_mode
	cout << "matrix destroyed" << endl;
#endif
}


bool matrix::IsInit(){
	return ninit;
}