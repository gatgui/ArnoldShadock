#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BinaryMathColorMtd);

enum BinaryMathColorParams
{
   p_operator = 0,
   p_input1,
   p_input2
};

node_parameters
{
   AiParameterEnum("operator", BMO_ADD, BinaryMathOperatorNames);
   AiParameterRGB("input1", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input2", 1.0f, 1.0f, 1.0f);
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
   
   AtRGB input1 = AiShaderEvalParamRGB(p_input1);
   AtRGB input2 = AiShaderEvalParamRGB(p_input2);
   
   switch (op)
   {
   case BMO_DIVIDE:
      sg->out.RGB = input1 / input2;
      return;
   case BMO_MAX:
      sg->out.RGB.r = (input1.r < input2.r ? input2.r : input1.r);
      sg->out.RGB.g = (input1.g < input2.g ? input2.g : input1.g);
      sg->out.RGB.b = (input1.b < input2.b ? input2.b : input1.b);
      return;
   case BMO_MIN:
      sg->out.RGB.r = (input1.r > input2.r ? input2.r : input1.r);
      sg->out.RGB.g = (input1.g > input2.g ? input2.g : input1.g);
      sg->out.RGB.b = (input1.b > input2.b ? input2.b : input1.b);
      return;
   case BMO_MULTIPLY:
      sg->out.RGB = input1 * input2;
      return;
   case BMO_SUBTRACT:
      sg->out.RGB = input1 - input2;
      return;
   case BMO_ADD:
   default:
      sg->out.RGB = input1 + input2;
      return;
   }
}
