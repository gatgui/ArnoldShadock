/*
MIT License
Copyright (c) 2014~ Gaetan Guidet
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(CompareFMtd);

enum CompareFParams
{
   p_operator = 0,
   p_invert,
   p_input1,
   p_input2,
   p_precision
};

node_parameters
{
   AiParameterEnum(SSTR::_operator, 0, CompareOperatorNames);
   AiParameterBool("invert", false);
   AiParameterFlt("input1", 0.0f);
   AiParameterFlt("input2", 0.0f);
   AiParameterFlt("precision", AI_EPSILON);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *data = (int*) AiNodeGetLocalData(node);
   *data = AiNodeGetInt(node, SSTR::_operator);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

shader_evaluate
{
   bool rv = false;
   
   CompareOperator op = (CompareOperator) *((int*) AiNodeGetLocalData(node));
   
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
   
   sg->out.BOOL() = (AiShaderEvalParamBool(p_invert) ? !rv : rv);
}
