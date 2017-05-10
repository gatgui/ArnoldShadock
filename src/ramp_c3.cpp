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

AI_SHADER_NODE_EXPORT_METHODS(RampC3Mtd);

enum RampC3Params
{
   p_input = 0,
   p_positions,
   p_values,
   p_interpolations,
   p_default_value
};

node_parameters
{
   AtArray *def;
   
   AiParameterFlt("input", 0.0f);
   
   def = AiArrayAllocate(2, 1, AI_TYPE_FLOAT);
   AiArraySetFlt(def, 0, 0.0f);
   AiArraySetFlt(def, 1, 1.0f);
   AiParameterArray(SSTR::positions, def);
   
   def = AiArrayAllocate(2, 1, AI_TYPE_RGB);
   AiArraySetRGB(def, 0, AI_RGB_BLACK);
   AiArraySetRGB(def, 1, AI_RGB_WHITE);
   AiParameterArray(SSTR::values, def);
   
   def = AiArrayAllocate(2, 1, AI_TYPE_INT);
   AiArraySetInt(def, 0, RI_Linear);
   AiArraySetInt(def, 1, RI_Linear);
   AiParameterArray(SSTR::interpolations, def);
   AiParameterRGB("default_value", 0.0f, 0.0f, 0.0f);
}

struct RampC3Data
{
   bool valid;
   unsigned int nkeys;
   AtArray *positions;
   bool evalPositions;
   AtArray *values;
   bool evalValues;
   AtArray *interpolations;
   bool evalInterpolations;
   unsigned int nshuffles;
   unsigned int **shuffles;
};

node_initialize
{
   RampC3Data *data = new RampC3Data();
   AddMemUsage<RampC3Data>();
   
   data->valid = false;
   data->nkeys = 0;
   data->positions = 0;
   data->evalPositions = false;
   data->values = 0;
   data->evalValues = false;
   data->interpolations = 0;
   data->evalInterpolations = false;
   data->nshuffles = 0;
   data->shuffles = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   RampC3Data *data = (RampC3Data*) AiNodeGetLocalData(node);
   
   data->valid = false;
   data->nkeys = 0;
   
   if (data->shuffles)
   {
      for (unsigned int i=0; i<data->nshuffles; ++i)
      {
         AiFree(data->shuffles[i]);
      }
      AiFree(data->shuffles);
      data->shuffles = 0;
   }
   data->nshuffles = 0;
   
   data->positions = AiNodeGetArray(node, SSTR::positions);
   data->evalPositions = AiNodeIsLinked(node, SSTR::positions);
   
   data->values = AiNodeGetArray(node, SSTR::values);
   data->evalValues = AiNodeIsLinked(node, SSTR::values);
   
   data->interpolations = AiNodeGetArray(node, SSTR::interpolations);
   data->evalInterpolations = AiNodeIsLinked(node, SSTR::interpolations);
   
   if (data->positions->nelements > 0 ||
       data->positions->nelements == data->values->nelements ||
       data->positions->nelements == data->interpolations->nelements)
   {
      data->nkeys = data->positions->nelements;
      
      data->nshuffles = (data->evalPositions ? GetRenderThreadsCount() : 1);
      data->shuffles = (unsigned int **) AiMalloc(data->nshuffles * sizeof(unsigned int*));
      
      for (unsigned int i=0; i<data->nshuffles; ++i)
      {
         data->shuffles[i] = (unsigned int*) AiMalloc(data->nkeys * sizeof(unsigned int));
      }
      
      if (!data->evalPositions)
      {
         SortRampPositions(data->positions, data->shuffles[0]);
      }
      
      data->valid = true;
   }
   else
   {
      AiMsgWarning("[ramp_color] Invalid ramp parameters");
   }
}

node_finish
{
   RampC3Data *data = (RampC3Data*) AiNodeGetLocalData(node);
   
   for (unsigned int i=0; i<data->nshuffles; ++i)
   {
      AiFree(data->shuffles[i]);
   }
   AiFree(data->shuffles);
   
   delete data;
   SubMemUsage<RampC3Data>();
}

shader_evaluate
{
   RampC3Data *data = (RampC3Data*) AiNodeGetLocalData(node);
   
   if (!data->valid)
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
      return;
   }
   
   AtArray *p = (data->evalPositions ? AiShaderEvalParamArray(p_positions) : data->positions);
   AtArray *v = (data->evalValues ? AiShaderEvalParamArray(p_values) : data->values);
   AtArray *i = (data->evalInterpolations ? AiShaderEvalParamArray(p_interpolations) : data->interpolations);
   
   if (v->nelements != data->nkeys || p->nelements != data->nkeys || i->nelements != data->nkeys)
   {
      AiMsgWarning("[ramp_color] Array size mismatch");
      sg->out.RGB = AiShaderEvalParamFlt(p_default_value);
   }
   else
   {
      float input = AiShaderEvalParamFlt(p_input);
      int si = 0;
      
      if (data->evalPositions)
      {
         si = sg->tid;
         SortRampPositions(i, data->shuffles[si]);
      }
      
      ColorRamp(p, v, i, RI_Linear, data->shuffles[si], input, sg->out.RGB);
   }
}
