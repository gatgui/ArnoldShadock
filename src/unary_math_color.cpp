#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UnaryMathColorMtd);

enum UnaryMathColorParams
{
   p_operator = 0,
   p_input
};

node_parameters
{
   AiParameterEnum("operator", UMO_ABS, UnaryMathOperatorNames);
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
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
   
   AtRGB input = AiShaderEvalParamRGB(p_input);
   
   switch (op)
   {
   case UMO_CEIL:
      sg->out.RGB.r = ceilf(input.r);
      sg->out.RGB.g = ceilf(input.g);
      sg->out.RGB.b = ceilf(input.b);
      return;
   case UMO_FLOOR:
      sg->out.RGB.r = floorf(input.r);
      sg->out.RGB.g = floorf(input.g);
      sg->out.RGB.b = floorf(input.b);
      return;
   case UMO_INVERSE:
      sg->out.RGB.r = 1.0f / input.r;
      sg->out.RGB.g = 1.0f / input.g;
      sg->out.RGB.b = 1.0f / input.b;
      return;
   case UMO_NEGATE:
      sg->out.RGB.r = -input.r;
      sg->out.RGB.g = -input.g;
      sg->out.RGB.b = -input.b;
      return;
   case UMO_ROUND:
      sg->out.RGB.r = floorf(input.r + 0.5f);
      sg->out.RGB.g = floorf(input.g + 0.5f);
      sg->out.RGB.b = floorf(input.b + 0.5f);
      return;
   case UMO_SIGN:
      sg->out.RGB.r = (input.r < 0.0f ? -1.0f : 1.0f);
      sg->out.RGB.g = (input.g < 0.0f ? -1.0f : 1.0f);
      sg->out.RGB.b = (input.b < 0.0f ? -1.0f : 1.0f);
      return;
   case UMO_ABS:
   default:
      sg->out.RGB.r = (input.r < 0.0f ? -input.r : input.r);
      sg->out.RGB.g = (input.g < 0.0f ? -input.g : input.g);
      sg->out.RGB.b = (input.b < 0.0f ? -input.b : input.b);
      return;
   }
}
