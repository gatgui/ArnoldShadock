#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BinaryMathColorMtd);

enum BinaryMathColorParams
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
   AiParameterRGB("input1", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input2", 0.0f, 0.0f, 0.0f);
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
   BinaryMathOperator op = (BinaryMathOperator) AiShaderEvalParamInt(p_operator);
   
   AtRGB input1 = AiShaderEvalParamRGB(p_input1);
   AtRGB input2 = AiShaderEvalParamRGB(p_input2);
   bool abort_on_error = AiShaderEvalParamBool(p_abort_on_error);
   AtRGB error_value = AiShaderEvalParamRGB(p_error_value);
   
   switch (op)
   {
   case BMO_ADD:
      sg->out.RGB = input1 + input2;
      return;
   case BMO_DIVIDE:
      if ((-AI_EPSILON < input2.r && input2.r < AI_EPSILON) ||
          (-AI_EPSILON < input2.g && input2.g < AI_EPSILON) ||
          (-AI_EPSILON < input2.b && input2.b < AI_EPSILON))
      {
         if (abort_on_error)
         {
            AiRenderAbort();
         }
         sg->out.RGB = error_value;
      }
      else
      {
         sg->out.RGB = input1 / input2;
      }
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
   default:
      if (abort_on_error)
      {
         AiRenderAbort();
      }
      sg->out.RGB = error_value;
   }
}
