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
   AiParameterEnum("weight", LW_diffuse, LightWeightNames);
   
   AiMetaDataSetBool(mds, "weight", "linkable", false);
}

node_initialize
{
}

node_update
{
   AiNodeSetLocalData(node, reinterpret_cast<void*>(AiNodeGetInt(node, "weight")));
}

node_finish
{
}

shader_evaluate
{
   void* weight = AiNodeGetLocalData(node);
   
   if (sg->Lp)
   {
      if (weight == (void*)LW_diffuse)
      {
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
      }
      else if (weight == (void*)LW_specular)
      {
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
      }
      else if (weight == (void*)LW_sss)
      {
         sg->out.FLT = AiLightGetSSS(sg->Lp);
         return;
      }
   }
   
   sg->out.FLT = 0.0f;
}
