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

AI_SHADER_NODE_EXPORT_METHODS(RefractVMtd);

enum RefractVParams
{
   p_direction = 0,
   p_normal,
   p_ior1,
   p_ior2
};

node_parameters
{
   AiParameterVec(SSTR::direction, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::normal, 0.0f, 0.0f, 0.0f);
   AiParameterFlt(SSTR::ior1, 1.0f);
   AiParameterFlt(SSTR::ior2, 1.0f);
}

struct RefractVData
{
   bool evalN;
   float ior1;
   float ior2;
};

node_initialize
{
   AiNodeSetLocalData(node, new RefractVData());
   AddMemUsage<RefractVData>();
}

node_update
{
   RefractVData *data = (RefractVData*) AiNodeGetLocalData(node);
   
   data->evalN = AiNodeIsLinked(node, SSTR::normal);
   data->ior1 = AiNodeGetFlt(node, SSTR::ior1);
   data->ior2 = AiNodeGetFlt(node, SSTR::ior2);
}

node_finish
{
   RefractVData *data = (RefractVData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<RefractVData>();
}

shader_evaluate
{
   RefractVData *data = (RefractVData*) AiNodeGetLocalData(node);
   
   AtVector D = AiShaderEvalParamVec(p_direction);
   AtVector N = (data->evalN ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   if (!AiRefract(&D, &N, &(sg->out.VEC), data->ior1, data->ior2))
   {
      // Total Internal Reflection
      sg->out.VEC = AI_V3_ZERO;
   }
}
