#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(LightWeightMtd);

enum LightWeightParams
{
   p_weight = 0
};

enum LightWeight
{
   LW_diffuse = 0,
   LW_specular,
   LW_sss
};

static const char* LightWeightNames[] =
{
   "diffuse",
   "specular",
   "sss",
   NULL
};

node_parameters
{
   AiParameterEnum(SSTR::weight, LW_diffuse, LightWeightNames);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *pw = (int*) AiNodeGetLocalData(node);
   *pw = AiNodeGetInt(node, SSTR::weight);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

shader_evaluate
{
   int weight = *((int*) AiNodeGetLocalData(node));
   
   if (sg->Lp)
   {
      switch (weight)
      {
      case LW_diffuse:
         // Note: AiLightGetAffectDiffuse API is set deprecated
         //       but still in use by MtoA
         if (AiLightGetAffectDiffuse(sg->Lp))
         {
            sg->out.FLT = AiLightGetDiffuse(sg->Lp);
         }
         else
         {
            sg->out.FLT = 0.0f;
         }
         return;
      
      case LW_specular:
         // Note: AiLightGetAffectSpecular is set deprecated
         //       but still in use by MtoA
         if (AiLightGetAffectSpecular(sg->Lp))
         {
            sg->out.FLT = AiLightGetSpecular(sg->Lp);
         }
         else
         {
            sg->out.FLT = 0.0f;
         }
         return;
      
      case LW_sss:
         sg->out.FLT = AiLightGetSSS(sg->Lp);
         return;
      
      default:
         break;
      }
   }
   
   sg->out.FLT = 0.0f;
}
