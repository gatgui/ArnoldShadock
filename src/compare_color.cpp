#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(CompareColorMtd);

enum CompareColorParams
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
   AiParameterRGB("input1", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input2", 0.0f, 0.0f, 0.0f);
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
   
   AtRGB input1 = AiShaderEvalParamRGB(p_input1);
   AtRGB input2 = AiShaderEvalParamRGB(p_input2);
   float tmp;
   
   switch (op)
   {
   case CO_EQ:
      rv = true;
      tmp = input1.r - input2.r;
      rv = rv && (tmp < 0.0f ? (tmp > -precision) : (tmp < precision));
      tmp = input1.g - input2.g;
      rv = rv && (tmp < 0.0f ? (tmp > -precision) : (tmp < precision));
      tmp = input1.b - input2.b;
      rv = rv && (tmp < 0.0f ? (tmp > -precision) : (tmp < precision));
      break;
   case CO_NEQ:
      rv = true;
      tmp = input1.r - input2.r;
      rv = rv && (tmp < 0.0f ? (tmp <= -precision) : (tmp >= precision));
      tmp = input1.g - input2.g;
      rv = rv && (tmp < 0.0f ? (tmp <= -precision) : (tmp >= precision));
      tmp = input1.b - input2.b;
      rv = rv && (tmp < 0.0f ? (tmp <= -precision) : (tmp >= precision));
      break;
   case CO_LT:
      rv = (input1.r < input2.r && input1.g < input2.g && input1.b < input2.b);
      break;
   case CO_LTE:
      rv = (input1.r <= input2.r && input1.g <= input2.g && input1.b <= input2.b);
      break;
   case CO_GT:
      rv = (input1.r > input2.r && input1.g > input2.g && input1.b > input2.b);
      break;
   case CO_GTE:
      rv = (input1.r >= input2.r && input1.g >= input2.g && input1.b >= input2.b);
      break;
   default:
      break;
   }
   
   sg->out.BOOL = (AiShaderEvalParamBool(p_invert) ? !rv : rv);
}
