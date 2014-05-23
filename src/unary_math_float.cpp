#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UnaryMathFloatMtd);

enum UnaryMathFloatParams
{
   p_operator = 0,
   p_input,
   p_abort_on_error,
   p_error_value
};

node_parameters
{
   AiParameterEnum("operator", UMO_ABS, UnaryMathOperatorNames);
   AiParameterFlt("input", 0.0f);
   AiParameterBool("abort_on_error", false);
   AiParameterFlt("error_value", 0.0f);
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
   bool abort_on_error = AiShaderEvalParamBool(p_abort_on_error);
   float error_value = AiShaderEvalParamFlt(p_error_value);
   
   switch (op)
   {
   case UMO_ABS:
      sg->out.FLT = (input < 0.0f ? -input : input);
      return;
   case UMO_CEIL:
      sg->out.FLT = ceilf(input);
      return;
   case UMO_FLOOR:
      sg->out.FLT = floorf(input);
      return;
   case UMO_INVERSE:
      if (-AI_EPSILON < input && input < AI_EPSILON)
      {
         if (abort_on_error)
         {
            AiRenderAbort();
         }
         sg->out.FLT = error_value;
      }
      else
      {
         sg->out.FLT = 1.0f / input;
      }
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
   default:
      if (abort_on_error)
      {
         AiRenderAbort();
      }
      sg->out.FLT = error_value;
   }
}
