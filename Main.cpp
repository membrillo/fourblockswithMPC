#include <string>
#include <iostream>
#include "jpMatrix.h"
#include "linalg.h"

matrix zeros(UINT rowss, UINT colss){
	matrix ret(rowss, colss);
	for (size_t i = 0; i < rowss; ++i){
		for (size_t j = 0; j < colss; ++j){
			ret(i, j) = 0;
		}
	}
	return ret;
}

matrix inverse(matrix &from_1){
	UINT n;
	matrix ret(from_1.nrows,from_1.ncols);
	alglib::ae_int_t info;
	alglib::matinvreport rep;
	alglib::real_2d_array a, b;
	if (from_1.nrows == from_1.ncols){
		try{
			n = from_1.nrows;
			a.setlength(n, n);
			for (size_t i = 0; i < n; ++i){
				for(size_t j = 0; j < n; ++j){
					a[i][j] = from_1(i, j);
				}
			}
			alglib::rmatrixinverse(a, info,rep);
			if (int(info) != 1) throw jp_error(INVERSION_INFO_ERROR);
			cout << endl << "matrix inversion info: " << int(info) << endl;
			cout << a.tostring(2).c_str() << endl;
			for (size_t i = 0; i < n; ++i){
				for (size_t j = 0; j < n; ++j){
					ret(i,j) = a[i][j];
				}
			}
			ret.ToString();
			return ret;
		}
		catch (alglib::ap_error error){
			cout << error.msg << endl;
		}
	}
	else{
		throw jp_error(NOT_SQUARE_MATRIX);
	}
	UINT rows, cols;
	

}

matrix diag(matrix &a){
	matrix ret;
	
	if (a.ncols == 1){
		//ret.Init(a.nrows, a.nrows);
		ret = zeros(a.nrows, a.nrows);
		for (size_t i = 0; i < a.nrows; i++){
			ret(i, i) = a(i, 0);
		}
	}
	else if (a.nrows == 1){
		ret = zeros(a.ncols, a.ncols);
		for (size_t i = 0; i < a.ncols; i++){
			ret(i, i) = a(0, i);
		}
	}
	else{
		throw jp_error(DIAG_NOT_ONE_DIMENSION);
	}
	return ret;
}

void WriteMatrixOnFile(matrix &from_1,const char * path){
	//matrix to;
	UINT iCol, iRow;
	ofstream out;
	out.open(path);
	//out << "[ " << endl;
	out << from_1.nrows << "," << from_1.ncols << endl;
	for (size_t i = 0; i < from_1.ncols; i++){
		for(size_t j = 0; j < from_1.nrows; j++){
			out << from_1(i, j) << ",";
		}
		out << endl;
	}
	//out << " ]" << endl;
	
	out.close();
}

matrix ReadMatrixFF(const char *path){
	matrix to;
	double db, *db_2;
	UINT nrows, ncols;
	char a[100];
	ifstream f_ent;
	try{
		f_ent.open(path);
		f_ent.getline(a, 90, ',');
		nrows = atoi(a);
		f_ent.getline(a, 90);
		ncols = atoi(a);
		to.Init(nrows, ncols);
		cout << nrows << "," << ncols << endl;
		for (size_t i = 0; i < nrows; i++){
			for (size_t j = 0; j < ncols; j++){
				if (j == ncols - 1){
					f_ent.getline(a, 90);
					to(i, j) = atof(a);
					cout << a;
					break;
				}
				f_ent.getline(a, 90, ',');
				to(i, j) = atof(a);
				cout << a << ',';
			}
			cout << endl;
		}
		to.ToString();
		f_ent.close();
	}
	catch (exception e){
		cout << e.what() << endl;
		throw(jp_error(READ_FF_ERROR));
	}
	return to;
}

matrix jpMakePTDF(matrix &branch){
	matrix A, B, Ybus, ret;
	matrix aux_1, aux_2, aux_3, aux_4,aux_5;
	UINT nrows, ncols, count, nl;
	nrows = branch.nrows;
	A = zeros(nrows, nrows);
	nl = nrows;
	count = 0;
	cout << "intbranch: " << int(branch(count, 0)) << endl;
	cout << "A: (" << A.nrows << "," << A.ncols << ")" <<endl;
	while (count < nl){
		A(count, int(branch(count, 0))-1) = 1;
		A(count, int(branch(count, 1))-1) = -1;
		count++;
	}
	aux_1.Init(nrows, 1);
	for (size_t i = 0; i < nrows; i++){
		aux_1(i, 0) = 1/branch(i, 3);
	}
	B = diag(aux_1);
	aux_2 = B*A;
	aux_3 = A.t()*aux_2;
	aux_4.Init(nrows - 1, nrows - 1);
	for (size_t i = 0; i < nrows - 1; i++){
		for (size_t j = 0; j < nrows - 1; j++){
			aux_4(i, j) = aux_3(i + 1, j + 1);
		}
	}
	aux_4 = inverse(aux_4);
	for (size_t i = 1; i < nrows; i++){
		for (size_t j = 1; j < nrows; j++){
			aux_3(i, j) = aux_4(i - 1, j - 1);
		}
	}
	for (size_t i = 0; i < nrows; i++){
		aux_3(i, 0) = 0;
		aux_3(0, i) = 0;
	}
	
	aux_3.ToString();
	A.ToString();
	aux_5 = A*aux_3;
	ret = B*aux_5;
	ret.ToString();
	return ret;
}

matrix jpMakePTDF_2(matrix &branch){
	matrix A, B, Ybus, ret;
	matrix aux_1, aux_2, aux_3, aux_4, aux_5;
	UINT nrows, ncols, count, nl;
	nrows = branch.nrows;
	A = zeros(nrows, nrows);
	nl = nrows;
	count = 0;
	//cout << "intbranch: " << int(branch(count, 0)) << endl;
	//cout << "A: (" << A.nrows << "," << A.ncols << ")" << endl;
	while (count < nl){
		A(count, int(branch(count, 0)) - 1) = 1;
		A(count, int(branch(count, 1)) - 1) = -1;
		count++;
	}
	aux_1.Init(nrows, 1);
	for (size_t i = 0; i < nrows; i++){
		aux_1(i, 0) = 1 / branch(i, 3);
	}
	B = diag(aux_1);
	aux_1.~matrix();
	aux_2 = B*A;
	aux_3 = A.t()*aux_2;
	aux_2.~matrix();
	aux_4.Init(nrows - 1, nrows - 1);
	for (size_t i = 0; i < nrows - 1; i++){
		for (size_t j = 0; j < nrows - 1; j++){
			aux_4(i, j) = aux_3(i + 1, j + 1);
		}
	}
	aux_4 = inverse(aux_4);
	for (size_t i = 1; i < nrows; i++){
		for (size_t j = 1; j < nrows; j++){
			aux_3(i, j) = aux_4(i - 1, j - 1);
		}
	}
	aux_4.~matrix();
	for (size_t i = 0; i < nrows; i++){
		aux_3(i, 0) = 0;
		aux_3(0, i) = 0;
	}

	//aux_3.ToString();
	//A.ToString();
	aux_5 = A*aux_3;
	ret = B*aux_5;
	//ret.ToString();
	return ret;
}

int main(){
	UINT dos = 2;
	UINT uno = 2;
	alglib::real_2d_array dda;
	matrix a(2, 2), b(2, 2), c, d, e, ff, test_1,test_linemat, PTDF;
	double k[2][2] = { { 4, 5 }, { 2, 6 } };
	double k1[17][1] = { { 432 }, { 5 }, { 4 }, { 3 }, { 21 }, { 3 }, { 4 }, { 5 }, { 4 }, { 5 }, { 13 }, { 1 }, { 2 }, { 4 }, { 3 }, { 2 }, { 1 } };
	try{
		a(0, 0) = 1;
		a(1, 0) = 4;
		a(0, 1) = 3;
		a(1, 1) = 5;
		b(0, 0) = 4;
		b(1, 0) = 4;
		b(0, 1) = 7;
		b(1, 1) = 5;
		c = a*b;
		cout << c(0, 0)<<endl;
		cout << c(0, 1);
		c = a.t();
		cout << endl << c(0, 1) << endl;
		d.Init(k, 2, 2);
		cout << d(1, 1) << endl;
		d.ToString();
		cout << "-------------------------------" << endl;
		ff.Init(k1, 17, 1);
		e = diag(ff);
		e.ToString();
		WriteMatrixOnFile(e ,"nepe.jpmatrix");
		test_1 = inverse(e);
		WriteMatrixOnFile(test_1, "nepe_inverted.jpmatrix");
		ReadMatrixFF("nepe.jpmatrix");
		test_linemat = ReadMatrixFF("LineMatrix.jpmatrix");
		PTDF = jpMakePTDF(test_linemat);
		PTDF.ToString();
	}
	catch (jp_error erd){
		cout << erd.msg << endl;
	}
	cin.get();
	return 0;
}