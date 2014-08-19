#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TraceStateColorMtd);

enum TraceStateColorParams
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
   AiParameterEnum("state", TS_color, TraceStateNames);
   AiParameterBool("trace", false);
   AiParameterRGB("default", 0.0f, 0.0f, 0.0f);
   
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
         AiMsgWarning("[trace_state_color] Trying to access result from a 'probe' trace: Use 'probe_state_vector' instead");
      }
      sg->out.RGB = AiShaderEvalParamRGB(p_default);
   }
   else
   {
      void *data = AiNodeGetLocalData(node);
      TraceState state = (TraceState) *((int*)&data);
      
      sample = (AtScrSample*) hit->ptr;
      
      switch (state)
      {
      case TS_color:
         sg->out.RGB = sample->color;
         break;
      case TS_opacity:
         sg->out.RGB = sample->opacity;
         break;
      default:
         sg->out.RGB = AiShaderEvalParamRGB(p_default);
         break;
      }
   }
}
