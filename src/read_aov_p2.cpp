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

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVP2Mtd);

enum ReadAOVP2Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterPnt2("default_value", 0.0f, 0.0f);
}

struct ReadAOVP2Data
{
   AtString aovName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadAOVP2Data());
   AddMemUsage<ReadAOVP2Data>();
}

node_update
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadAOVP2Data>();
}

shader_evaluate
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetPnt2(sg, data->aovName, sg->out.PNT2))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default_value);
   }
}
