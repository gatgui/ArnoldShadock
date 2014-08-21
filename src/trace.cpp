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
   
   AiMetaDataSetBool(mds, "type", "linkable", false);
}

struct TraceData
{
   TraceType type;
   int count;
   HitData *hits;
};

node_initialize
{
   TraceData *data = (TraceData*) AiMalloc(sizeof(TraceData));
   data->type = TT_standard;
   data->count = 0;
   data->hits = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   TraceData *data = (TraceData*) AiNodeGetLocalData(node);
   
   TraceType type = (TraceType) AiNodeGetInt(node, "type");
   
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
   
   AiFree(data);
}

shader_evaluate
{
   TraceData *data = (TraceData*) AiNodeGetLocalData(node);
   
   AtRay *ray = 0;
   HitData *hit = 0;
   
   AiStateSetMsgPtr("agsb_ray", 0);
   
   AiShaderEvalParamVec(p_ray);
   
   if (!AiStateGetMsgPtr("agsb_ray", (void**)&ray) || !ray)
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
   
   AiStateSetMsgPtr("agsb_trace_hit", (sg->out.BOOL ? hit : 0));
}
