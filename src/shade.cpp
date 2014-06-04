#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShadeMtd);

enum ShadeParams
{
   p_mode = 0,
   p_override_normal,
   p_normal
};

enum ShadeMode
{
   SM_DirectDiffuse = 0,
   SM_IndirectDiffuse
};

static const char* ShadeModeNames[] =
{
   "direct_diffuse",
   "indirect_diffuse",
   NULL
};

node_parameters
{
   AiParameterEnum("mode", SM_DirectDiffuse, ShadeModeNames);
   AiParameterBool("override_normal", false);
   AiParameterVec("normal", 1.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "mode", "linkable", false);
   AiMetaDataSetBool(mds, "override_normal", "linkable", false);
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
   ShadeMode mode = (ShadeMode) AiShaderEvalParamInt(p_mode);
   
   AtVector N = (AiShaderEvalParamBool(p_override_normal) ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   switch (mode)
   {
   case SM_DirectDiffuse:
      sg->out.RGB = AiDirectDiffuse(&N, sg);
      break;
   case SM_IndirectDiffuse:
      sg->out.RGB = AiIndirectDiffuse(&N, sg);
      break;
   default:
      sg->out.RGB = AI_RGB_BLACK;
   }
}
