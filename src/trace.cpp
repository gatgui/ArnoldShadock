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
   AiParameterEnum("type", TT_standard, TraceTypeNames);
}

struct TraceData
{
   AtShaderGlobals **hits;
   AtScrSample **samples;
   int count;
};

node_initialize
{
   TraceData *data = (TraceData*) AiMalloc(sizeof(TraceData));
   data->hits = 0;
   data->samples = 0;
   data->count = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   TraceData *data = (TraceData*) AiNodeGetLocalData(node);
   
   int nthreads = GetRenderThreadsCount();
   
   if (nthreads != data->count)
   {
      AtShaderGlobals **hits = (AtShaderGlobals**) AiMalloc(nthreads * sizeof(AtShaderGlobals*));
      AtScrSample **samples = (AtScrSample**) AiMalloc(nthreads * sizeof(AtScrSample*));
      
      int cc = (nthreads < data->count ? nthreads : data->count);
      
      for (int i=0; i<cc; ++i)
      {
         hits[i] = data->hits[i];
         samples[i] = data->samples[i];
      }
      
      for (int i=cc; i<data->count; ++i)
      {
         AiShaderGlobalsDestroy(data->hits[i]);
         AiFree(data->samples[i]);
      }
      
      for (int i=cc; i<nthreads; ++i)
      {
         hits[i] = AiShaderGlobals();
         samples[i] = (AtScrSample*) AiMalloc(sizeof(AtScrSample));
      }
      
      AiFree(data->hits);
      AiFree(data->samples);
      
      data->hits = hits;
      data->samples = samples;
      data->count = nthreads;
   }
}

node_finish
{
   TraceData *data = (TraceData*) AiNodeGetLocalData(node);
   
   if (data->count > 0)
   {
      for (int i=0; i<data->count; ++i)
      {
         AiShaderGlobalsDestroy(data->hits[i]);
         AiFree(data->samples[i]);
      }
      AiFree(data->hits);
      AiFree(data->samples);
   }
   AiFree(data);
}

shader_evaluate
{
   TraceData *data = (TraceData*) AiNodeGetLocalData(node);
   
   AtRay *ray = 0;
   
   AiStateSetMsgPtr("ray", 0);
   
   AiShaderEvalParamVec(p_ray);
   
   if (!AiStateGetMsgPtr("ray", (void**)&ray) || !ray)
   {
      AiStateSetMsgPtr("trace_sample", 0);
      sg->out.BOOL = false;
   }
   else
   {
      TraceType type = (TraceType) AiShaderEvalParamInt(p_type);
      
      AtScrSample *sample = 0;
      AtShaderGlobals *hit = 0;
      
      switch (type)
      {
      case TT_probe:
         hit = data->hits[sg->tid];
         sg->out.BOOL = AiTraceProbe(ray, hit);
         break;
      case TT_background:
         sample = data->samples[sg->tid];
         AiTraceBackground(ray, sample);
         sg->out.BOOL = true;
         break;
      case TT_standard:
      default:
         sample = data->samples[sg->tid];
         sg->out.BOOL = AiTrace(ray, sample);
      }
      
      AiStateSetMsgPtr("trace_sample", (sg->out.BOOL ? sample : 0));
      AiStateSetMsgPtr("trace_hit", (sg->out.BOOL ? hit : 0));
   }
}
