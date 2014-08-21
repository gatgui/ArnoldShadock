#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TraceStateFloatMtd);

enum TraceStateFloatParams
{
   p_state = 0,
   p_trace,
   p_default
};

enum TraceState
{
   TS_alpha = 0,
   TS_z
};

static const char* TraceStateNames[] =
{
   "alpha",
   "z",
   NULL
};

node_parameters
{
   AiParameterEnum("state", TS_alpha, TraceStateNames);
   AiParameterBool("trace", false);
   AiParameterFlt("default", 0.0f);
   
   AiMetaDataSetBool(mds, "state", "linkable", false);
}

node_initialize
{
}

node_update
{
   AiNodeSetLocalData(node, (void*) AiNodeGetInt(node, "state"));
}

node_finish
{
}

shader_evaluate
{
   AtScrSample *sample = 0;
   HitData *hit = 0;
   
   if (!AiShaderEvalParamBool(p_trace) ||
       !AiStateGetMsgPtr("agsb_trace_hit", (void**)&hit) ||
       !hit ||
       hit->isSG)
   {
      if (hit->isSG)
      {
         AiMsgWarning("[trace_state_float] Trying to access result from a 'probe' trace: Use 'probe_state_vector' instead");
      }
      sg->out.FLT = AiShaderEvalParamFlt(p_default);
   }
   else
   {
      void *data = AiNodeGetLocalData(node);
      TraceState state = (TraceState) size_t(data);
      
      sample = (AtScrSample*) hit->ptr;
      
      switch (state)
      {
      case TS_alpha:
         sg->out.FLT = sample->alpha;
         break;
      case TS_z:
         sg->out.FLT = sample->z;
         break;
      default:
         sg->out.FLT = AiShaderEvalParamFlt(p_default);
         break;
      }
   }
}
