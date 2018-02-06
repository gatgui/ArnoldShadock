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

AI_SHADER_NODE_EXPORT_METHODS(WriteMsgC4Mtd);

enum WriteMsgC4Params
{
   p_input = 0,
   p_msg_name,
   p_msg_input,
   p_eval_order
};

node_parameters
{
   AiParameterClosure("input");
   
   AiParameterStr(SSTR::msg_name, "");
   AiParameterRGBA(SSTR::msg_input, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterEnum(SSTR::eval_order, EO_input_last, EvalOrderNames);
}

struct WriteMsgC4Data
{
   bool valid;
   AtString msgName;
   EvalOrder evalOrder;
};

node_initialize
{
   WriteMsgC4Data *data = new WriteMsgC4Data();
   AddMemUsage<WriteMsgC4Data>();
   
   data->valid = false;
   data->evalOrder = EO_input_last;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   WriteMsgC4Data *data = (WriteMsgC4Data*) AiNodeGetLocalData(node);
   
   data->evalOrder = (EvalOrder) AiNodeGetInt(node, SSTR::eval_order);
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
   data->valid = !data->msgName.empty();
}

node_finish
{
   WriteMsgC4Data *data = (WriteMsgC4Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<WriteMsgC4Data>();
}

shader_evaluate
{
   WriteMsgC4Data *data = (WriteMsgC4Data*) AiNodeGetLocalData(node);
   
   if (data->evalOrder == EO_input_first)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(p_input);
   }
   
   if (data->valid)
   {
      AiStateSetMsgRGBA(data->msgName, AiShaderEvalParamRGBA(p_msg_input));
   }
   
   if (data->evalOrder == EO_input_last)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(p_input);
   }
}
