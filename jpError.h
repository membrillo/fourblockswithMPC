#ifndef JP_ERROR
#define JP_ERROR
#include <string>
//matrix internal functions errors:
#define OUT_OF_INDEX 100
#define DOT_COL_ROW  101
#define NOT_EQUAL    103
#define MATRIX_IS_ALREADY_INIT 104
#define NOT_SQUARE_MATRIX 105
//matrix external functions errors:
#define DIAG_NOT_ONE_DIMENSION 204
#define READ_FF_ERROR		   205
#define INVERSION_INFO_ERROR   206

typedef unsigned int UINT;

class jp_error{


public:
	std::string msg;
	jp_error(UINT err_code);
	~jp_error(){};
};


#endif