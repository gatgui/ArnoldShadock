#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BinaryMathFloatMtd);

enum BinaryMathFloatParams
{
   p_operator = 0,
   p_input1,
   p_input2,
   p_abort_on_error,
   p_error_value
};

node_parameters
{
   AiParameterEnum("operator", UMO_ABS, BinaryMathOperatorNames);
   AiParameterFlt("input1", 0.0f);
   AiParameterFlt("input1", 0.0f);
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
   BinaryMathOperator op = (BinaryMathOperator) AiShaderEvalParamInt(p_operator);
   
   float input1 = AiShaderEvalParamFlt(p_input1);
   float input2 = AiShaderEvalParamFlt(p_input2);
   bool abort_on_error = AiShaderEvalParamBool(p_abort_on_error);
   float error_value = AiShaderEvalParamFlt(p_error_value);
   
   switch (op)
   {
   case BMO_ADD:
      sg->out.FLT = input1 + input2;
      return;
   case BMO_DIVIDE:
      if (-AI_EPSILON < input2 && input2 < AI_EPSILON)
      {
         if (abort_on_error)
         {
            AiRenderAbort();
         }
         sg->out.FLT = error_value;
      }
      else
      {
         sg->out.FLT = input1 / input2;
      }
      return;
   case BMO_MAX:
      sg->out.FLT = (input1 < input2 ? input2 : input1);
      return;
   case BMO_MIN:
      sg->out.FLT = (input1 > input2 ? input2 : input1);
      return;
   case BMO_MULTIPLY:
      sg->out.FLT = input1 * input2;
      return;
   case BMO_SUBTRACT:
      sg->out.FLT = input1 - input2;
      return;
   default:
      if (abort_on_error)
      {
         AiRenderAbort();
      }
      sg->out.FLT = error_value;
   }
}
