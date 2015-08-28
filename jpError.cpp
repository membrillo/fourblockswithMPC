#include "jpError.h"

jp_error::jp_error(UINT err_code){
	switch (err_code){
	case OUT_OF_INDEX:
		msg = "ERROR 100: out of index";
		break;
	case DOT_COL_ROW:
		msg = "ERROR 101: from.1.rows != from_2.cols";
		break;
	case DIAG_NOT_ONE_DIMENSION:
		msg = "ERROR 201: the matrix is 2D";
		break;
	case MATRIX_IS_ALREADY_INIT:
		msg = "ERROR 104: the matrix is already initialized";
		break;
	case NOT_SQUARE_MATRIX:
		msg = "ERROR 105: the matrix is not square";
		break;
	case  READ_FF_ERROR:
		msg = "ERROR 205: error while reading the file";
		break;
	case INVERSION_INFO_ERROR:
		msg = "ERROR 206: internal error of LinAlg while inverting the matrix";
		break;
	default:
		msg = "NULL";
	}
}