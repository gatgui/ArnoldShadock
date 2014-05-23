#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BinaryMathVectorMtd);

enum BinaryMathVectorParams
{
   p_operator = 0,
   p_input1,
   p_input2
};

node_parameters
{
   AiParameterEnum("operator", BMO_ADD, BinaryMathOperatorNames);
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 1.0f, 1.0f, 1.0f);
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
   
   switch (op)
   {
   case BMO_DIVIDE:
      sg->out.VEC = input1 / input2;
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
   case BMO_ADD:
   default:
      sg->out.VEC = input1 + input2;
      return;
   }
}
