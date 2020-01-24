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

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgBMtd);

enum ReadMsgBParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterBool("default_value", false);
}

struct ReadMsgBData
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgBData());
   AddMemUsage<ReadMsgBData>();
}

node_update
{
   ReadMsgBData *data = (ReadMsgBData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgBData *data = (ReadMsgBData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgBData>();
}

shader_evaluate
{
   ReadMsgBData *data = (ReadMsgBData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgBool(data->msgName, &(sg->out.BOOL())))
   {
      sg->out.BOOL() = AiShaderEvalParamBool(p_default_value);
   }
}
