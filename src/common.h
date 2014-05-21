#ifndef __agUtils_common_h__
#define __agUtils_common_h__

#include <ai.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

extern const char* CompareOperatorNames[];

enum CompareOperator
{
   CO_EQ = 0,
   CO_NEQ,
   CO_LT,
   CO_LTE,
   CO_GT,
   CO_GTE
};

#endif
