#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TraceResultFMtd);

enum TraceResultFParams
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
   AiParameterEnum(SSTR::state, TS_alpha, TraceStateNames);
   AiParameterBool("trace", false);
   AiParameterFlt("default", 0.0f);
   
   AiMetaDataSetBool(mds, SSTR::state, SSTR::linkable, false);
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
       !AiStateGetMsgPtr("agsb_trace_hit", (void**)&hit) ||
       !hit ||
       hit->isSG)
   {
      if (hit && hit->isSG)
      {
         AiMsgWarning("[trace_state_float] Trying to access result from a 'probe' trace: Use 'probe_state_vector' instead");
      }
      sg->out.FLT = AiShaderEvalParamFlt(p_default);
   }
   else
   {
      TraceState state = (TraceState) *((int*) AiNodeGetLocalData(node));
      
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
