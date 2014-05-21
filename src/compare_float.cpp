#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(CompareFloatMtd);

enum CompareFloatParams
{
   p_operator = 0,
   p_invert,
   p_input1,
   p_input2,
   p_precision
};

node_parameters
{
   AiParameterEnum("operator", 0, CompareOperatorNames);
   AiParameterBool("invert", false);
   AiParameterFlt("input1", 0.0f);
   AiParameterFlt("input2", 0.0f);
   AiParameterFlt("precision", AI_EPSILON);
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
   bool rv = false;
   
   CompareOperator op = (CompareOperator) AiShaderEvalParamInt(p_operator);
   
   float precision = AiShaderEvalParamFlt(p_precision);
   
   float input1 = AiShaderEvalParamFlt(p_input1);
   float input2 = AiShaderEvalParamFlt(p_input2);
   float tmp;
   
   switch (op)
   {
   case CO_EQ:
      tmp = input1 - input2;
      rv = (tmp < 0.0f ? (tmp > -precision) : (tmp < precision));
      break;
   case CO_NEQ:
      tmp = input1 - input2;
      rv = (tmp < 0.0f ? (tmp <= -precision) : (tmp >= precision));
      break;
   case CO_LT:
      rv = (input1 < input2);
      break;
   case CO_LTE:
      rv = (input1 <= input2);
      break;
   case CO_GT:
      rv = (input1 > input2);
      break;
   case CO_GTE:
      rv = (input1 >= input2);
      break;
   default:
      break;
   }
   
   sg->out.BOOL = (AiShaderEvalParamBool(p_invert) ? !rv : rv);
}
