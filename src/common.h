#ifndef __agUtils_common_h__
#define __agUtils_common_h__

#include <ai.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

extern const char* CompareOperatorNames[];
extern const char* UnaryMathOperatorNames[];
extern const char* BinaryMathOperatorNames[];

enum CompareOperator
{
   CO_EQ = 0,
   CO_NEQ,
   CO_LT,
   CO_LTE,
   CO_GT,
   CO_GTE
};

enum UnaryMathOperator
{
   UMO_ABS = 0,
   UMO_CEIL,
   UMO_FLOOR,
   UMO_INVERSE,
   UMO_NEGATE,
   UMO_ROUND,
   UMO_SIGN
};

enum BinaryMathOperator
{
   BMO_ADD = 0,
   BMO_DIVIDE,
   BMO_MAX,
   BMO_MIN,
   BMO_MULTIPLY,
   BMO_SUBTRACT
};

#endif
