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
   
   if (!AiShaderEvalParamBool(p_trace) ||
       !AiStateGetMsgPtr("trace_hit", (void**)&sample) ||
       !sample)
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default);
   }
   else
   {
      void *data = AiNodeGetLocalData(node);
      TraceState state = (TraceState) *((int*)&data);
      
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
