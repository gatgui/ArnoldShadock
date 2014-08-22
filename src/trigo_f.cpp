#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TrigoFMtd);

enum TrigoFParams
{
   p_input1 = 0,
   p_input2,
   p_operation,
   p_units
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
   AiParameterEnum("operation", TO_COS, TrigonometryOperationNames);
   AiParameterEnum("units", AU_Radians, AngleUnitsNames);
   
   AiMetaDataSetBool(mds, "operation", "linkable", false);
   AiMetaDataSetBool(mds, "units", "linkable", false);
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
   AngleUnits units = (AngleUnits) AiShaderEvalParamInt(p_units);
   TrigonometryOperation op = (TrigonometryOperation) AiShaderEvalParamInt(p_operation);
   
   switch (op)
   {
   case TO_COS:
      sg->out.FLT = cosf((units == AU_Degrees ? sDegToRad : 1.0f) * input);
      break;
   case TO_SIN:
      sg->out.FLT = sinf((units == AU_Degrees ? sDegToRad : 1.0f) * input);
      break;
   case TO_TAN:
      sg->out.FLT = tanf((units == AU_Degrees ? sDegToRad : 1.0f) * input);
      break;
   case TO_ACOS:
      sg->out.FLT = (units == AU_Degrees ? sRadToDeg : 1.0f) * acosf(input);
      break;
   case TO_ASIN:
      sg->out.FLT = (units == AU_Degrees ? sRadToDeg : 1.0f) * asinf(input);
      break;
   case TO_ATAN:
      sg->out.FLT = (units == AU_Degrees ? sRadToDeg : 1.0f) * atanf(input);
      break;
   case TO_ATAN2:
      sg->out.FLT = (units == AU_Degrees ? sRadToDeg : 1.0f) * atan2f(input, AiShaderEvalParamFlt(p_input2));
      break;
   default:
      sg->out.FLT = 0.0f;
   }
}
