#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UnaryMathColorMtd);

enum UnaryMathColorParams
{
   p_operator = 0,
   p_input,
   p_abort_on_error,
   p_error_value
};

node_parameters
{
   AiParameterEnum("operator", UMO_ABS, UnaryMathOperatorNames);
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterBool("abort_on_error", false);
   AiParameterRGB("error_value", 0.0f, 0.0f, 0.0f);
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
   bool abort_on_error = AiShaderEvalParamBool(p_abort_on_error);
   AtRGB error_value = AiShaderEvalParamRGB(p_error_value);
   
   switch (op)
   {
   case UMO_ABS:
      sg->out.RGB.r = (input.r < 0.0f ? -input.r : input.r);
      sg->out.RGB.g = (input.g < 0.0f ? -input.g : input.g);
      sg->out.RGB.b = (input.b < 0.0f ? -input.b : input.b);
      return;
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
      if ((-AI_EPSILON < input.r && input.r < AI_EPSILON) ||
          (-AI_EPSILON < input.g && input.g < AI_EPSILON) ||
          (-AI_EPSILON < input.b && input.b < AI_EPSILON))
      {
         if (abort_on_error)
         {
            AiRenderAbort();
         }
         sg->out.RGB = error_value;
      }
      else
      {
         sg->out.RGB.r = 1.0f / input.r;
         sg->out.RGB.g = 1.0f / input.g;
         sg->out.RGB.b = 1.0f / input.b;
      }
      return;
   case UMO_OPPOSITE:
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
   default:
      if (abort_on_error)
      {
         AiRenderAbort();
      }
      sg->out.RGB = error_value;
   }
}
