#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include "mpc.h"
#include <ctype.h>
class MpcReader
{
public:
	MpcReader(std::string pathIn);
	mpc& getMpc();
	void read();
	~MpcReader();

	static const int NONE = 0;
	static const int VARIABLE_DECLARATION = 1;
	static const int MATRIX_DECLARATION = 2;
	static const int MATRIX_END_DECLARATION = 3;
	static const int MATRIX_CONTINUE = 4;
	static const int MATRIX_SINGLE_LINE = 5;
	static const int VECTOR_DECLARATION = 6;
	static const int ALPHA_DECLARATION = 7;


private:
	

	std::size_t matrixDeclarationFound;
	std::size_t equalFound;
	std::size_t matrixEndFound;
	std::size_t endOfLineFound;
	std::size_t startParenthesisFound;
	std::size_t endParenthesisFound;
	std::string path;
	std::vector <std::string> variableNames;
	std::vector <std::size_t> variableFounds;
	mpc mpc;
	void split(std::string, std::string delimeter);
	std::string getName(std::string name);
	int detectTypeOfLine(std::string line);

	bool doesHaveNumbersToAdd(std::string line);
	double getValueFromLine(std::string line);
	double getValueFromLine(std::string line, int startPosition);
	bool isValidObjectDeclaration();
	bool matrixFound = false;

};

