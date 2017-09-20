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

AI_SHADER_NODE_EXPORT_METHODS(SampleLoopFMtd);

enum SampleLoopFParams
{
   p_sampler = 0,
   p_input,
   p_combine
};

node_parameters
{
   AiParameterRGB("sampler", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("input", 0.0f);
   AiParameterEnum(SSTR::combine, CM_Add, CombineModeNames);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *data = (int*) AiNodeGetLocalData(node);
   *data = AiNodeGetInt(node, SSTR::combine);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

shader_evaluate
{
   AtSampler *sampler = 0;
   
   AiShaderEvalParamRGB(p_sampler);
   
   if (!AiStateGetMsgPtr(SSTR::agsb_sampler, (void**)&sampler) || !sampler)
   {
      sg->out.RGB() = AI_RGB_BLACK;
   }
   else
   {
      AtVector2 sample;
      
      CombineMode combine = (CombineMode) *((int*) AiNodeGetLocalData(node));
      
      sg->out.FLT() = 0.0f;
      
      AtSamplerIterator *it = AiSamplerIterator(sampler, sg);
      
      while (AiSamplerGetSample(it, &(sample.x)))
      {
         AiStateSetMsgVec2(SSTR::agsb_sample_value, sample);
         
         float input = AiShaderEvalParamFlt(p_input);
         
         switch (combine)
         {
         case CM_Max:
            sg->out.FLT() = AiMax(input, sg->out.FLT());
            break;
         case CM_Min:
            sg->out.FLT() = AiMin(input, sg->out.FLT());
            break;
         case CM_Add:
         case CM_Avg:
         default:
            sg->out.FLT() += input;
            break;
         }
      }
      
      if (combine == CM_Avg)
      {
         sg->out.FLT() *= AiSamplerGetSampleInvCount(it);
      }
   }
}
