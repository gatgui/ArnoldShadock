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

AI_SHADER_NODE_EXPORT_METHODS(FresnelC3Mtd);

enum FresnelC3Params
{
   p_view_vector = 0,
   p_normal,
   p_reflectance
};

node_parameters
{
   AiParameterVec(SSTR::view_vector, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::normal, 0.0f, 0.0f, 0.0f);
   AiParameterRGB(SSTR::reflectance, 1.0f, 1.0f, 1.0f);
}

struct FresnelC3Data
{
   bool evalV;
   bool evalN;
   AtColor reflectance;
};

node_initialize
{
   AiNodeSetLocalData(node, new FresnelC3Data());
   AddMemUsage<FresnelC3Data>();
}

node_update
{
   FresnelC3Data *data = (FresnelC3Data*) AiNodeGetLocalData(node);
   
   data->evalV = AiNodeIsLinked(node, SSTR::view_vector);
   data->evalN = AiNodeIsLinked(node, SSTR::normal);
   data->reflectance = AiNodeGetRGB(node, SSTR::reflectance);
}

node_finish
{
   FresnelC3Data *data = (FresnelC3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<FresnelC3Data>();
}

shader_evaluate
{
   FresnelC3Data *data = (FresnelC3Data*) AiNodeGetLocalData(node);
   
   AtVector V = (data->evalV ? AiShaderEvalParamVec(p_view_vector) : sg->Rd);
   AtVector N = (data->evalN ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   AiFresnelWeightRGB(&N, &V, &(data->reflectance), &(sg->out.RGB));
}
