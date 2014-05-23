#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UnaryMathVectorMtd);

enum UnaryMathVectorParams
{
   p_operator = 0,
   p_input,
   p_abort_on_error,
   p_error_value
};

node_parameters
{
   AiParameterEnum("operator", UMO_ABS, UnaryMathOperatorNames);
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterBool("abort_on_error", false);
   AiParameterVec("error_value", 0.0f, 0.0f, 0.0f);
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
   UnaryMathOperator op = (UnaryMathOperator) AiShaderEvalParamInt(p_operator);
   
   AtVector input = AiShaderEvalParamVec(p_input);
   bool abort_on_error = AiShaderEvalParamBool(p_abort_on_error);
   AtVector error_value = AiShaderEvalParamVec(p_error_value);
   
   switch (op)
   {
   case UMO_ABS:
      sg->out.VEC.x = (input.x < 0.0f ? -input.x : input.x);
      sg->out.VEC.y = (input.y < 0.0f ? -input.y : input.y);
      sg->out.VEC.z = (input.z < 0.0f ? -input.z : input.z);
      return;
   case UMO_CEIL:
      sg->out.VEC.x = ceilf(input.x);
      sg->out.VEC.y = ceilf(input.y);
      sg->out.VEC.z = ceilf(input.z);
      return;
   case UMO_FLOOR:
      sg->out.VEC.x = floorf(input.x);
      sg->out.VEC.y = floorf(input.y);
      sg->out.VEC.z = floorf(input.z);
      return;
   case UMO_INVERSE:
      if ((-AI_EPSILON < input.x && input.x < AI_EPSILON) ||
          (-AI_EPSILON < input.y && input.y < AI_EPSILON) ||
          (-AI_EPSILON < input.z && input.z < AI_EPSILON))
      {
         if (abort_on_error)
         {
            AiRenderAbort();
         }
         sg->out.VEC = error_value;
      }
      else
      {
         sg->out.VEC.x = 1.0f / input.x;
         sg->out.VEC.y = 1.0f / input.y;
         sg->out.VEC.z = 1.0f / input.z;
      }
      return;
   case UMO_NEGATE:
      sg->out.VEC.x = -input.x;
      sg->out.VEC.y = -input.y;
      sg->out.VEC.z = -input.z;
      return;
   case UMO_ROUND:
      sg->out.VEC.x = floorf(input.x + 0.5f);
      sg->out.VEC.y = floorf(input.y + 0.5f);
      sg->out.VEC.z = floorf(input.z + 0.5f);
      return;
   case UMO_SIGN:
      sg->out.VEC.x = (input.x < 0.0f ? -1.0f : 1.0f);
      sg->out.VEC.y = (input.y < 0.0f ? -1.0f : 1.0f);
      sg->out.VEC.z = (input.z < 0.0f ? -1.0f : 1.0f);
      return;
   default:
      if (abort_on_error)
      {
         AiRenderAbort();
      }
      sg->out.VEC = error_value;
   }
}
