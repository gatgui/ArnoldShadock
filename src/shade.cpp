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
   AiParameterEnum(SSTR::mode, SM_DirectDiffuse, ShadeModeNames);
   AiParameterBool(SSTR::override_normal, false);
   AiParameterVec(SSTR::normal, 1.0f, 0.0f, 0.0f);
}

struct ShadeData
{
   int mode;
   bool overrideNormal;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShadeData());
   AddMemUsage<ShadeData>();
}

node_update
{
   ShadeData *data = (ShadeData*) AiNodeGetLocalData(node);
   data->mode = AiNodeGetInt(node, SSTR::mode);
   data->overrideNormal = AiNodeGetBool(node, SSTR::override_normal);
}

node_finish
{
   ShadeData *data = (ShadeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShadeData>();
}

shader_evaluate
{
   ShadeData *data = (ShadeData*) AiNodeGetLocalData(node);
   
   AtVector N = (data->overrideNormal ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   switch (data->mode)
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
