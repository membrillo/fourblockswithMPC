
#include "mpcMatrix.h"
#include <iostream>
#include <fstream>


mpcMatrix::mpcMatrix()
{
	name = "Null";
	rows = 0;
	cols = 0;
	isValid = false;
	array2D.clear();
}

mpcMatrix::mpcMatrix(std::string newName, int newCols, int newRows)
{
	name = newName;
	rows = newRows;
	cols = newCols;
	isValid = false;

	resize(rows, cols);
	

}

mpcMatrix::mpcMatrix(std::string newName)
{
	
	name = newName;
	rows = 0;
	cols = 0;
	isValid = false;
	array2D.clear();
	std::cout << "creada matriz de nombre: {" << name << "}" << std::endl;
}


void mpcMatrix::resize(int Newrows, int Newcols){
	rows = Newrows;
	cols = Newcols;
	array2D.resize(cols);
	for (int i = 0; i < cols; ++i)
		array2D[i].resize(rows);
}

void mpcMatrix::clearMatrix(){
	rows = 0;
	cols = 0;
	array2D.clear();

}






void mpcMatrix::changeName(std::string newName){
	name = newName;
}



void mpcMatrix::printMatrix(){
	std::cout << "Nombre : "<< name << " rows: "  << rows << " cols: " << cols<< std::endl;
	for (int y = 0; y < rows; y++){
		for (int x = 0; x < cols; x++){
			std::cout << "[" << array2D[x][y] << "]";
		}
		std::cout << std::endl;
	}
}


void mpcMatrix::addFromString(std::string line){
	std::cout << line << std::endl;
}

void mpcMatrix::addRowFromVector(std::vector <double> rowVector){
	if (rowVector.size() > cols){
			resize(rows+1, rowVector.size());
	}
	else{
		resize(rows + 1, cols);
	}

	for (int i = 0; i < rowVector.size(); i++){
		array2D[i][rows-1]=rowVector.at(i);
		//std::cout << "[" << array2D[i][rows - 1] << "]";
		
	}
	//std::cout << " agregada " << std::endl;
}

double& mpcMatrix::operator()(const int n, const int m){
	if (n >= rows || m >= cols){
		throw;
	}
	else{
		return   array2D[n][m]; 
	}

};

void mpcMatrix::operator=(mpcMatrix matrix){
	clearMatrix();
	resize(matrix.rows,matrix.cols);

	for (int y = 0; y < matrix.rows; y++){
		for (int x = 0; x < matrix.cols; x++){
			array2D[x][y] = matrix.array2D[x][y];
		}
	}
}

double** mpcMatrix::getAsDynamicArray(){
	double** aux=0;
	aux = new double*[rows];

	for (int h = 0; h < rows; h++)
	{
		aux[h] = new double[cols];

		for (int w = 0; w < cols; w++)
		{
			aux[h][w] = array2D[h][w];
		}
	}
	return aux;
}

std::vector <double> mpcMatrix::getCol(int columnIndex){
	std::vector <double> aux;
	for (int i = 0; i < rows; i++){
		aux.push_back(array2D[columnIndex-1][i]);
	}
	return aux;
}

std::vector <double> mpcMatrix::getRow(int rowIndex){
	std::vector <double> aux;
	for (int i = 0; i < rows; i++){
		aux.push_back(array2D[i][rowIndex-1]);
	}
	return aux;
}







void mpcMatrix::setRow(std::vector <double> rowVector, int rowIndex){
	if (rowIndex>rows){
		resize(rowIndex, rows);

	}
	if (rowVector.size() > cols){
		resize(rows, rowVector.size());
	}

	for (int i = 0; i < rowVector.size(); i++){
		array2D[i][rowIndex - 1] = rowVector.at(i);
	}
}

void mpcMatrix::setColumn(std::vector <double> columnsVector, int columnIndex, double factor){
	if (columnIndex>cols){
		resize(rows, columnIndex);
	}

	if (columnsVector.size() > rows){
		resize(columnsVector.size(), cols);
	}

	for (int i = 0; i < columnsVector.size(); i++){
		array2D[columnIndex - 1][i] = columnsVector.at(i)*factor;
	}
}

void mpcMatrix::addColumnFromVector(std::vector <double> columnsVector, double factor){
	resize(rows, cols + 1);
	std::vector <double> aux;
	for (int i = 0; i < columnsVector.size(); i++){
		aux.push_back(columnsVector.at(i)*factor);
	}
	setColumn(aux, cols);
}

void mpcMatrix::setColumn(std::vector <double> columnsVector, int columnIndex){
	
	if (columnIndex>cols){
		resize(rows, columnIndex);
	}
	
	if (columnsVector.size() > rows){
		resize(columnsVector.size(), cols);
	}

	for (int i = 0; i < columnsVector.size(); i++){
		array2D[columnIndex-1][i]=columnsVector.at(i);
	}
}




mpcMatrix::~mpcMatrix()
{
	
}
