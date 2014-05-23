#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BinaryMathFloatMtd);

enum BinaryMathFloatParams
{
   p_operator = 0,
   p_input1,
   p_input2
};

node_parameters
{
   AiParameterEnum("operator", BMO_ADD, BinaryMathOperatorNames);
   AiParameterFlt("input1", 0.0f);
   AiParameterFlt("input2", 1.0f);
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
   
   switch (op)
   {
   case BMO_DIVIDE:
      sg->out.FLT = input1 / input2;
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
   case BMO_ADD:
   default:
      sg->out.FLT = input1 + input2;
   }
}
