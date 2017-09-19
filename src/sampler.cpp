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

AI_SHADER_NODE_EXPORT_METHODS(SamplerMtd);

enum SamplerParams
{
   p_samples = 0,
   p_seeded,
   p_seed
};

node_parameters
{
   AiParameterInt(SSTR::samples, 1);
   AiParameterBool(SSTR::seeded, false);
   AiParameterInt(SSTR::seed, 0);
}

node_initialize
{
   AiNodeSetLocalData(node, 0);
}

node_update
{
   AtSampler *sampler = (AtSampler*) AiNodeGetLocalData(node);
   
   if (sampler)
   {
      AiSamplerDestroy(sampler);
   }
   
   if (AiNodeGetBool(node, SSTR::seeded))
   {
      sampler = AiSampler(AiNodeGetInt(node, SSTR::seed), AiNodeGetInt(node, SSTR::samples), 2);
   }
   else
   {
      static const uint32_t seed = static_cast<uint32_t>(AiNodeEntryGetNameAtString(AiNodeGetNodeEntry(node)).hash());
      sampler = AiSampler(seed, AiNodeGetInt(node, SSTR::samples), 2);
   }
   
   AiNodeSetLocalData(node, sampler);
}

node_finish
{
   AtSampler *sampler = (AtSampler*) AiNodeGetLocalData(node);
   
   if (sampler)
   {
      AiSamplerDestroy(sampler);
   }
}

shader_evaluate
{
   AiStateSetMsgPtr(SSTR::agsb_sampler, AiNodeGetLocalData(node));
   sg->out.RGB() = AI_RGB_BLACK;
}
