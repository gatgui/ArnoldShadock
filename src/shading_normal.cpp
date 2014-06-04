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
   
   AtPoint P, Ndummy;
   
   switch (mode)
   {
   case SN_geometric:
      AiShaderGlobalsGetPositionAtTime(sg, AiShaderEvalParamFlt(p_time), &P, &Ndummy, &(sg->out.VEC), &Ndummy);
      break;
   case SN_smoothed:
      AiShaderGlobalsGetPositionAtTime(sg, AiShaderEvalParamFlt(p_time), &P, &Ndummy, &Ndummy, &(sg->out.VEC));
      break;
   default:
      AiShaderGlobalsGetPositionAtTime(sg, AiShaderEvalParamFlt(p_time), &P, &(sg->out.VEC), &Ndummy, &Ndummy);
   }
   
}
