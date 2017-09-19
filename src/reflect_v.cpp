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

AI_SHADER_NODE_EXPORT_METHODS(ReflectVMtd);

enum ReflectVParams
{
   p_direction = 0,
   p_normal,
   p_safe,
   p_geometric_normal
};

node_parameters
{
   AiParameterVec(SSTR::direction, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::normal, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::safe, false);
   AiParameterVec(SSTR::geometric_normal, 0.0f, 0.0f, 0.0f);
}

struct ReflectVData
{
   bool evalN;
   bool safe;
   bool evalNg;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReflectVData());
   AddMemUsage<ReflectVData>();
}

node_update
{
   ReflectVData *data = (ReflectVData*) AiNodeGetLocalData(node);
   
   data->evalN = AiNodeIsLinked(node, SSTR::normal);
   data->evalNg = AiNodeIsLinked(node, SSTR::geometric_normal);
   data->safe = AiNodeGetBool(node, SSTR::safe);
}

node_finish
{
   ReflectVData *data = (ReflectVData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReflectVData>();
}

shader_evaluate
{
   ReflectVData *data = (ReflectVData*) AiNodeGetLocalData(node);
   
   AtVector D = AiShaderEvalParamVec(p_direction);
   AtVector N = (data->evalN ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   if (data->safe)
   {
      AtVector Ng = (data->evalNg ? AiShaderEvalParamVec(p_geometric_normal) : sg->Ng);
      sg->out.VEC() = AiReflectSafe(D, N, Ng);
   }
   else
   {
      sg->out.VEC() = AiReflect(D, N);
   }
}
