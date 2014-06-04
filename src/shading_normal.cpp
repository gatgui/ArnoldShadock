#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShadingNormalMtd);

enum ShadingNormalParams
{
   p_time = 0,
   p_mode
};

enum ShadingNormalMode
{
   SN_default = 0,
   SN_geometric,
   SN_smoothed
};

static const char* ShadingNormalModeNames[] =
{
   "default",
   "geometric",
   "smoothed",
   NULL
};

node_parameters
{
   AiParameterFlt("time", 0.0f);
   AiParameterEnum("mode", SN_default, ShadingNormalModeNames);
   
   AiMetaDataSetBool(mds, "mode", "linkable", false);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   ShadingNormalMode mode = (ShadingNormalMode) AiShaderEvalParamInt(p_mode);
   
   AtPoint P;
   AtVector N, Ns, Ng;
   
   AiShaderGlobalsGetPositionAtTime(sg, AiShaderEvalParamFlt(p_time), &P, &N, &Ng, &Ns);
   
   switch (mode)
   {
   case SN_geometric:
      sg->out.VEC = Ng;
      break;
   case SN_smoothed:
      sg->out.VEC = Ns;
      break;
   default:
      sg->out.VEC = N;
   }
   
}
