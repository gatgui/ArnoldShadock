#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TrigoVMtd);

enum TrigoVParams
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

extern const char* TrigonometryOperationNames[];

node_parameters
{
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 0.0f, 0.0f, 0.0f);
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
   
   AtVector input = AiShaderEvalParamVec(p_input1);
   AngleUnits units = (AngleUnits) AiShaderEvalParamInt(p_units);
   TrigonometryOperation op = (TrigonometryOperation) AiShaderEvalParamInt(p_operation);
   
   switch (op)
   {
   case TO_COS:
      {
         float f = (units == AU_Degrees ? sDegToRad : 1.0f);
         sg->out.VEC.x = cosf(f * input.x);
         sg->out.VEC.y = cosf(f * input.y);
         sg->out.VEC.z = cosf(f * input.z);
      }
      break;
   case TO_SIN:
      {
         float f = (units == AU_Degrees ? sDegToRad : 1.0f);
         sg->out.VEC.x = sinf(f * input.x);
         sg->out.VEC.y = sinf(f * input.y);
         sg->out.VEC.z = sinf(f * input.z);
      }
      break;
   case TO_TAN:
      {
         float f = (units == AU_Degrees ? sDegToRad : 1.0f);
         sg->out.VEC.x = tanf(f * input.x);
         sg->out.VEC.y = tanf(f * input.y);
         sg->out.VEC.z = tanf(f * input.z);
      }
      break;
   case TO_ACOS:
      {
         float f = (units == AU_Degrees ? sRadToDeg : 1.0f);
         sg->out.VEC.x = f * acosf(input.x);
         sg->out.VEC.y = f * acosf(input.y);
         sg->out.VEC.z = f * acosf(input.z);
      }
      break;
   case TO_ASIN:
      {
         float f = (units == AU_Degrees ? sRadToDeg : 1.0f);
         sg->out.VEC.x = f * asinf(input.x);
         sg->out.VEC.y = f * asinf(input.y);
         sg->out.VEC.z = f * asinf(input.z);
      }
      break;
   case TO_ATAN:
      {
         float f = (units == AU_Degrees ? sRadToDeg : 1.0f);
         sg->out.VEC.x = f * atanf(input.x);
         sg->out.VEC.y = f * atanf(input.y);
         sg->out.VEC.z = f * atanf(input.z);
      }
      break;
   case TO_ATAN2:
      {
         AtVector input2 = AiShaderEvalParamVec(p_input2);
         float f = (units == AU_Degrees ? sRadToDeg : 1.0f);
         sg->out.VEC.x = f * atan2f(input.x, input2.x);
         sg->out.VEC.y = f * atan2f(input.y, input2.y);
         sg->out.VEC.z = f * atan2f(input.z, input2.z);
      }
      break;
   default:
      sg->out.VEC.x = 0.0f;
      sg->out.VEC.y = 0.0f;
      sg->out.VEC.z = 0.0f;
   }
}
