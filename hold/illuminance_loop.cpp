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

AI_SHADER_NODE_EXPORT_METHODS(IlluminanceLoopMtd);

enum IlluminanceLoopParams
{
   p_input = 0,
   p_combine,
   p_reset_lights_cache
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::combine, CM_Add, CombineModeNames);
   AiParameterBool(SSTR::reset_lights_cache, false);
}

struct IlluminanceLoopData
{
   CombineMode combine;
   bool resetLightsCache;
};

node_initialize
{
   AiNodeSetLocalData(node, new IlluminanceLoopData());
   AddMemUsage<IlluminanceLoopData>();
}

node_update
{
   IlluminanceLoopData *data = (IlluminanceLoopData*) AiNodeGetLocalData(node);
   
   data->combine = (CombineMode) AiNodeGetInt(node, SSTR::combine);
   data->resetLightsCache = AiNodeGetBool(node, SSTR::reset_lights_cache);
}

node_finish
{
   IlluminanceLoopData *data = (IlluminanceLoopData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<IlluminanceLoopData>();
}

shader_evaluate
{
   IlluminanceLoopData *data = (IlluminanceLoopData*) AiNodeGetLocalData(node);
   
   if (data->resetLightsCache)
   {
      AiLightsResetCache(sg);
   }
   
   AiLightsPrepare(sg);
   
   sg->out.RGB() = AI_RGB_BLACK;
   
   float sampleCount = 0.0f;
   AtRGB sampleValue;
   AtLightSample ls;
   
   while (AiLightsGetSample(sg, ls))
   {
      sampleValue = AiShaderEvalParamRGB(p_input);
      switch (data->combine)
      {
      case CM_Max:
         sg->out.RGB().r = AiMax(sampleValue.r, sg->out.RGB().r);
         sg->out.RGB().g = AiMax(sampleValue.g, sg->out.RGB().g);
         sg->out.RGB().b = AiMax(sampleValue.b, sg->out.RGB().b);
         break;
      case CM_Min:
         sg->out.RGB().r = AiMin(sampleValue.r, sg->out.RGB().r);
         sg->out.RGB().g = AiMin(sampleValue.g, sg->out.RGB().g);
         sg->out.RGB().b = AiMin(sampleValue.b, sg->out.RGB().b);
         break;
      case CM_Add:
      case CM_Avg:
      default:
         sg->out.RGB() += sampleValue;
      }
      
      sampleCount += 1.0f;
   }
   
   if (data->combine == CM_Avg && sampleCount > 0.0f)
   {
      sampleCount = 1.0f / sampleCount;
      sg->out.RGB() *= sampleCount;
   }
}
