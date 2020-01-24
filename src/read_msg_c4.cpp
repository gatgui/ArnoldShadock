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

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgC4Mtd);

enum ReadMsgC4Params
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterRGBA("default_value", 0.0f, 0.0f, 0.0f, 1.0f);
}

struct ReadMsgC4Data
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgC4Data());
   AddMemUsage<ReadMsgC4Data>();
}

node_update
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgC4Data>();
}

shader_evaluate
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgRGBA(data->msgName, &(sg->out.RGBA())))
   {
      sg->out.RGBA() = AiShaderEvalParamRGBA(p_default_value);
   }
}
