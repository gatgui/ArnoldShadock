#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UnaryMathFloatMtd);

enum UnaryMathFloatParams
{
   p_operator = 0,
   p_input
};

node_parameters
{
   AiParameterEnum("operator", UMO_ABS, UnaryMathOperatorNames);
   AiParameterFlt("input", 0.0f);
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
   
   float input = AiShaderEvalParamFlt(p_input);
   
   switch (op)
   {
   case UMO_CEIL:
      sg->out.FLT = ceilf(input);
      return;
   case UMO_FLOOR:
      sg->out.FLT = floorf(input);
      return;
   case UMO_INVERSE:
      sg->out.FLT = 1.0f / input;
      return;
   case UMO_NEGATE:
      sg->out.FLT = -input;
      return;
   case UMO_ROUND:
      sg->out.FLT = floorf(input + 0.5f);
      return;
   case UMO_SIGN:
      sg->out.FLT = (input < 0.0f ? -1.0f : 1.0f);
      return;
   case UMO_ABS:
   default:
      sg->out.FLT = (input < 0.0f ? -input : input);
      return;
   }
}
