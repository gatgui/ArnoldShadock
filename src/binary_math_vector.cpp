#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BinaryMathVectorMtd);

enum BinaryMathVectorParams
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
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 0.0f, 0.0f, 0.0f);
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
   BinaryMathOperator op = (BinaryMathOperator) AiShaderEvalParamInt(p_operator);
   
   AtVector input1 = AiShaderEvalParamVec(p_input1);
   AtVector input2 = AiShaderEvalParamVec(p_input2);
   bool abort_on_error = AiShaderEvalParamBool(p_abort_on_error);
   AtVector error_value = AiShaderEvalParamVec(p_error_value);
   
   switch (op)
   {
   case BMO_ADD:
      sg->out.VEC = input1 + input2;
      return;
   case BMO_DIVIDE:
      if ((-AI_EPSILON < input2.x && input2.x < AI_EPSILON) ||
          (-AI_EPSILON < input2.y && input2.y < AI_EPSILON) ||
          (-AI_EPSILON < input2.z && input2.z < AI_EPSILON))
      {
         if (abort_on_error)
         {
            AiRenderAbort();
         }
         sg->out.VEC = error_value;
      }
      else
      {
         sg->out.VEC = input1 / input2;
      }
      return;
   case BMO_MAX:
      sg->out.VEC.x = (input1.x < input2.x ? input2.x : input1.x);
      sg->out.VEC.y = (input1.y < input2.y ? input2.y : input1.y);
      sg->out.VEC.z = (input1.z < input2.z ? input2.z : input1.z);
      return;
   case BMO_MIN:
      sg->out.VEC.x = (input1.x > input2.x ? input2.x : input1.x);
      sg->out.VEC.y = (input1.y > input2.y ? input2.y : input1.y);
      sg->out.VEC.z = (input1.z > input2.z ? input2.z : input1.z);
      return;
   case BMO_MULTIPLY:
      sg->out.VEC = input1 * input2;
      return;
   case BMO_SUBTRACT:
      sg->out.VEC = input1 - input2;
      return;
   default:
      if (abort_on_error)
      {
         AiRenderAbort();
      }
      sg->out.VEC = error_value;
   }
}
