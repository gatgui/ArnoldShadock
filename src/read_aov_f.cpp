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

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVFMtd);

enum ReadAOVFParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterFlt("default_value", 0.0f);
}

struct ReadAOVFData
{
   AtString aovName;
};

node_initialize
{
   ReadAOVFData *data = (ReadAOVFData*) AiMalloc(sizeof(ReadAOVFData));
   AiAddMemUsage(int64_t(sizeof(ReadAOVFData)), (AtString)"shading_blocks");
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVFData *data = (ReadAOVFData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVFData *data = (ReadAOVFData*) AiNodeGetLocalData(node);
   AiFree(data);
   AiAddMemUsage(-int64_t(sizeof(ReadAOVFData)), (AtString)"shading_blocks");
}

shader_evaluate
{
   ReadAOVFData *data = (ReadAOVFData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetFlt(sg, data->aovName, sg->out.FLT()))
   {
      sg->out.FLT() = AiShaderEvalParamFlt(p_default_value);
   }
}
