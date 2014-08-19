#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TraceStateVectorMtd);

enum TraceStateVectorParams
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
   AiParameterEnum("state", TS_point, TraceStateNames);
   AiParameterBool("trace", false);
   AiParameterVec("default", 0.0f, 0.0f, 0.0f);
   
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
   
   if (!AiShaderEvalParamBool(p_trace) ||
       !AiStateGetMsgPtr("trace_hit", (void**)&sample) ||
       !sample)
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default);
   }
   else
   {
      void *data = AiNodeGetLocalData(node);
      TraceState state = (TraceState) *((int*)&data);
      
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
