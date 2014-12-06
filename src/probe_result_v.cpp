#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ProbeResultVMtd);

enum ProbeResultVParams
{
   p_state = 0,
   p_trace,
   p_default
};

enum ProbeState
{
   PS_position = 0,
   PS_normal
};

static const char* ProbeStateNames[] =
{
   "position",
   "normal",
   NULL
};

node_parameters
{
   AiParameterEnum("state", PS_position, ProbeStateNames);
   AiParameterBool("trace", false);
   AiParameterVec("default", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "state", "linkable", false);
}

node_initialize
{
}

node_update
{
   AiNodeSetLocalData(node, reinterpret_cast<void*>(AiNodeGetInt(node, "state")));
}

node_finish
{
}

shader_evaluate
{
   AtShaderGlobals *hitsg = 0;
   HitData *hit = 0;
   
   if (!AiShaderEvalParamBool(p_trace) ||
       !AiStateGetMsgPtr("agsb_trace_hit", (void**)&hit) ||
       !hit ||
       !hit->isSG)
   {
      if (hit && !hit->isSG)
      {
         AiMsgWarning("[probe_state_vector] Trying to access result from a 'standard' or 'background' trace: Use 'trace_state_vector' instead");
      }
      sg->out.VEC = AiShaderEvalParamVec(p_default);
   }
   else
   {
      void *data = AiNodeGetLocalData(node);
      ProbeState state = (ProbeState) size_t(data);
      
      hitsg = (AtShaderGlobals*) hit->ptr;
      
      switch (state)
      {
      case PS_position:
         sg->out.VEC = hitsg->P;
         break;
      case PS_normal:
         sg->out.VEC = hitsg->N;
         break;
      default:
         sg->out.VEC = AiShaderEvalParamVec(p_default);
         break;
      }
   }
}
