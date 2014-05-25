#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TrigonometryFloatMtd);

enum TrigonometryFloatParams
{
   p_input1,
   p_input2,
   p_operation,
   p_units
};

enum TrigonometryUnits
{
   TU_DEGREES = 0,
   TU_RADIANS
};

enum TrigonometryOperation
{
   TO_ACOS = 0,
   TO_ASIN,
   TO_ATAN,
   TO_ATAN2,
   TO_COS,
   TO_SIN,
   TO_TAN
};

const char* TrigonometryUnitsNames[] =
{
   "degrees",
   "radians",
   NULL
};

const char* TrigonometryOperationNames[] =
{
   "acos",
   "asin",
   "atan",
   "atan2",
   "cos",
   "sin",
   "tan",
   NULL
};

node_parameters
{
   AiParameterFlt("input1", 0.0f);
   AiParameterFlt("input2", 0.0f);
   AiParameterENUM("operation", TO_COS, TrigonometryOperationNames);
   AiParameterENUM("units", TU_RADIANS, TrigonometryUnitsNames);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   static float sDegToRad = AI_PI / 180.0f;
   static float sRadToDeg = 180.0f / AI_PI;
   
   float input = AiShaderEvalParamFlt(p_input1);
   TrigonometryUnits units = (TrigonometryUnits) AiShaderEvalParamInt(p_units);
   TrigonometryOperation op = (TrigonometryOperation) AiShaderEvalParamInt(p_operation);
   
   switch (op)
   {
   case TO_COS:
      sg->out.FLT = cosf((units == TU_DEGREES ? sDegToRad : 1.0f) * input);
      break;
   case TO_SIN:
      sg->out.FLT = sinf((units == TU_DEGREES ? sDegToRad : 1.0f) * input);
      break;
   case TO_TAN:
      sg->out.FLT = tanf((units == TU_DEGREES ? sDegToRad : 1.0f) * input);
      break;
   case TO_ACOS:
      sg->out.FLT = (units == TU_DEGREES ? sRadToDeg : 1.0f) * acosf(input);
      break;
   case TO_ASIN:
      sg->out.FLT = (units == TU_DEGREES ? sRadToDeg : 1.0f) * asinf(input);
      break;
   case TO_ATAN:
      sg->out.FLT = (units == TU_DEGREES ? sRadToDeg : 1.0f) * atanf(input);
      break;
   case TO_ATAN2:
      sg->out.FLT = (units == TU_DEGREES ? sRadToDeg : 1.0f) * atan2f(input, AiShaderEvalParamFlt(p_input2));
      break;
   default:
      sg->out.FLT = 0.0f;
   }
}
