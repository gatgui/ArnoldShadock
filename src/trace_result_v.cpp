#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TraceResultVMtd);

enum TraceResultVParams
{
   p_state = 0,
   p_trace,
   p_default
};

enum TraceState
{
   TS_point = 0
};

static const char* TraceStateNames[] =
{
   "point",
   NULL
};

node_parameters
{
   AiParameterEnum(SSTR::state, TS_point, TraceStateNames);
   AiParameterBool("trace", false);
   AiParameterVec("default", 0.0f, 0.0f, 0.0f);
   
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
       !AiStateGetMsgPtr(SSTR::agsb_trace_hit, (void**)&hit) ||
       !hit ||
       hit->isSG)
   {
      if (hit && hit->isSG)
      {
         AiMsgWarning("[trace_state_vector] Trying to access result from a 'probe' trace: Use 'probe_state_vector' instead");
      }
      sg->out.VEC = AiShaderEvalParamVec(p_default);
   }
   else
   {
      TraceState state = (TraceState) *((int*) AiNodeGetLocalData(node));
      
      sample = (AtScrSample*) hit->ptr;
      
      switch (state)
      {
      case TS_point:
         sg->out.VEC = sample->point;
         break;
      default:
         sg->out.VEC = AiShaderEvalParamVec(p_default);
         break;
      }
   }
}
