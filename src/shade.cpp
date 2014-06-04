#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShadeMtd);

enum ShadeParams
{
   p_mode = 0,
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
   AiParameterVec("normal", 1.0f, 0.0f, 0.0f);
}

node_initialize
{
}

node_update
{
   if (!AiNodeIsLinked(node, "normal") &&
       !AiNodeIsLinked(node, "normal.x") &&
       !AiNodeIsLinked(node, "normal.y") && 
       !AiNodeIsLinked(node, "normal.z"))
   {
      // normal not linked at all, use sg->N in evaluate
      AiNodeSetLocalData(node, (void*)1);
   }
   else
   {
      AiNodeSetLocalData(node, (void*)0);
   }
}

node_finish
{
}

shader_evaluate
{
   ShadeMode mode = (ShadeMode) AiShaderEvalParamInt(p_mode);
   
   AtVector N = (AiNodeGetLocalData(node) == (void*)1 ? sg->N : AiShaderEvalParamVec(p_normal));
   
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
