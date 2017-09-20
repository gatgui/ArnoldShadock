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

AI_SHADER_NODE_EXPORT_METHODS(TraceResultC3Mtd);

enum TraceResultC3Params
{
   p_state = 0,
   p_trace,
   p_default
};

enum TraceState
{
   TS_color = 0,
   TS_opacity
};

static const char* TraceStateNames[] =
{
   "color",
   "opacity",
   NULL
};

node_parameters
{
   AiParameterEnum(SSTR::state, TS_color, TraceStateNames);
   AiParameterBool("trace", false);
   AiParameterRGB("default", 0.0f, 0.0f, 0.0f);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *data = (int*) AiNodeGetLocalData(node);
   *data = AiNodeGetInt(node, SSTR::state);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

shader_evaluate
{
   AtScrSample *sample = 0;
   HitData *hit = 0;
   
   if (!AiShaderEvalParamBool(p_trace) ||
       !AiStateGetMsgPtr(SSTR::agsb_trace_hit, (void**)&hit) ||
       !hit ||
       hit->isSG)
   {
      if (hit && hit->isSG)
      {
         AiMsgWarning("[trace_result_c3] Trying to access result from a 'probe' trace: Use 'probe_result_v' instead");
      }
      sg->out.RGB() = AiShaderEvalParamRGB(p_default);
   }
   else
   {
      TraceState state = (TraceState) *((int*) AiNodeGetLocalData(node));
      
      sample = (AtScrSample*) hit->ptr;
      
      switch (state)
      {
      case TS_color:
         sg->out.RGB() = sample->color;
         break;
      case TS_opacity:
         sg->out.RGB() = sample->opacity;
         break;
      default:
         sg->out.RGB() = AiShaderEvalParamRGB(p_default);
         break;
      }
   }
}
