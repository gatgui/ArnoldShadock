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

AI_SHADER_NODE_EXPORT_METHODS(TraceMtd);

enum TraceParams
{
   p_ray = 0,
   p_type
};

enum TraceType
{
   TT_standard = 0,
   TT_background,
   TT_probe
};

static const char* TraceTypeNames[] = {"standard", "background", "probe", NULL};

node_parameters
{
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::type, TT_standard, TraceTypeNames);
}

struct TraceData
{
   TraceType type;
   int count;
   HitData *hits;
};

node_initialize
{
   TraceData *data = new TraceData();
   AddMemUsage<TraceData>();
   
   data->type = TT_standard;
   data->count = 0;
   data->hits = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   TraceData *data = (TraceData*) AiNodeGetLocalData(node);
   
   TraceType type = (TraceType) AiNodeGetInt(node, SSTR::type);
   
   if (type != data->type)
   {
      // Change in ray type, deallocate all data
      if (data->count > 0)
      {
         if (data->type == TT_probe)
         {
            for (int i=0; i<data->count; ++i)
            {
               AiShaderGlobalsDestroy((AtShaderGlobals*) data->hits[i].ptr);
            }
         }
         else
         {
            for (int i=0; i<data->count; ++i)
            {
               AiFree(data->hits[i].ptr);
            }
         }
         
         AiFree(data->hits);
         
         data->hits = 0;
         data->count = 0;
      }
      
      data->type = type;
   }
   
   int nthreads = GetRenderThreadsCount();
   
   if (nthreads != data->count)
   {
      HitData *hits = (HitData*) AiMalloc(nthreads * sizeof(HitData));
      
      int cc = (nthreads < data->count ? nthreads : data->count);
      
      for (int i=0; i<cc; ++i)
      {
         hits[i] = data->hits[i];
      }
      
      if (data->type == TT_probe)
      {
         for (int i=cc; i<data->count; ++i)
         {
            AiShaderGlobalsDestroy((AtShaderGlobals*) data->hits[i].ptr);
         }
         for (int i=cc; i<nthreads; ++i)
         {
            hits[i].ptr = AiShaderGlobals();
            hits[i].isSG = true;
         }
      }
      else
      {
         for (int i=cc; i<data->count; ++i)
         {
            AiFree(data->hits[i].ptr);
         }
         for (int i=cc; i<nthreads; ++i)
         {
            hits[i].ptr = AiMalloc(sizeof(AtScrSample));
            hits[i].isSG = false;
         }
      }
      
      AiFree(data->hits);
      
      data->hits = hits;
      data->count = nthreads;
   }
}

node_finish
{
   TraceData *data = (TraceData*) AiNodeGetLocalData(node);
   
   if (data->count > 0)
   {
      if (data->type == TT_probe)
      {
         for (int i=0; i<data->count; ++i)
         {
            AiShaderGlobalsDestroy((AtShaderGlobals*) data->hits[i].ptr);
         }
      }
      else
      {
         for (int i=0; i<data->count; ++i) 
         {
            AiFree(data->hits[i].ptr);
         }
      }
      
      AiFree(data->hits);
   }
   
   delete data;
   SubMemUsage<TraceData>();
}

shader_evaluate
{
   TraceData *data = (TraceData*) AiNodeGetLocalData(node);
   
   AtRay *ray = 0;
   HitData *hit = 0;
   
   AiStateSetMsgPtr(SSTR::agsb_ray, 0);
   
   AiShaderEvalParamVec(p_ray);
   
   if (!AiStateGetMsgPtr(SSTR::agsb_ray, (void**)&ray) || !ray)
   {
      sg->out.BOOL = false;
   }
   else
   {
      hit = &(data->hits[sg->tid]);
      
      switch (data->type)
      {
      case TT_probe:
         sg->out.BOOL = AiTraceProbe(ray, (AtShaderGlobals*) hit->ptr);
         break;
      case TT_background:
         AiTraceBackground(ray, (AtScrSample*) hit->ptr);
         sg->out.BOOL = true;
         break;
      case TT_standard:
      default:
         sg->out.BOOL = AiTrace(ray, (AtScrSample*) hit->ptr);
      }
   }
   
   AiStateSetMsgPtr(SSTR::agsb_trace_hit, (sg->out.BOOL ? hit : 0));
}
