#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(EvaluateLightSampleMtd);

enum EvaluateLightSampleParams
{
   p_brdf = 0,
   p_surface_color,
   p_surface_color_weight,
   p_light_weight
};

enum LightWeight
{
   LW_none = 0,
   LW_diffuse,
   LW_specular,
   LW_sss
};

static const char* LightWeightNames[] =
{
   "none",
   "diffuse",
   "specular",
   "sss",
   NULL
};


node_parameters
{
   AiParameterRGB("brdf", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("surface_color", 1.0f, 1.0f, 1.0f);
   AiParameterFlt("surface_color_weight", 1.0f);
   AiParameterEnum("light_weight", LW_none, LightWeightNames);
   
   AiMetaDataSetBool(mds, "light_weight", "linkable", false);
}

struct EvaluateLightSampleData
{
   bool surface_color_is_linked;
   AtColor surface_color;
   
   bool surface_color_weight_is_linked;
   float surface_color_weight;
   
   LightWeight light_weight;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(EvaluateLightSampleData)));
}

node_update
{
   EvaluateLightSampleData *data = (EvaluateLightSampleData*) AiNodeGetLocalData(node);
   
   data->surface_color_is_linked = AiNodeIsLinked(node, "surface_color");
   if (!data->surface_color_is_linked)
   {
      data->surface_color = AiNodeGetRGB(node, "surface_color");
   }
   
   data->surface_color_weight_is_linked = AiNodeIsLinked(node, "surface_color_weight");
   if (!data->surface_color_weight_is_linked)
   {
      data->surface_color_weight = AiNodeGetFlt(node, "surface_color_weight");
   }
   
   data->light_weight = (LightWeight) AiNodeGetInt(node, "light_weight");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   EvaluateLightSampleData *data = (EvaluateLightSampleData*) AiNodeGetLocalData(node);
   
   BRDFData *brdf = 0;
   
   AiStateSetMsgPtr("brdf", 0);
   
   AiShaderEvalParamRGB(p_brdf);
   
   if (!AiStateGetMsgPtr("brdf", (void**)&brdf) || !brdf)
   {
      sg->out.RGB = AI_RGB_BLACK;
   }
   else
   {
      if (!data->surface_color_is_linked && AiColorIsZero(data->surface_color))
      {
         sg->out.RGB = AI_RGB_BLACK;
         return;
      }
      
      if (!data->surface_color_weight_is_linked && data->surface_color_weight <= 0.0f)
      {
         sg->out.RGB = AI_RGB_BLACK;
         return;
      }
      
      float lw = 0.0f;
      
      if (sg->Lp)
      {
         switch (data->light_weight)
         {
         case LW_diffuse:
            if (AiLightGetAffectDiffuse(sg->Lp))
            {
               lw = AiLightGetDiffuse(sg->Lp);
            }
            break;
         case LW_specular:
            if (AiLightGetAffectSpecular(sg->Lp))
            {
               lw = AiLightGetSpecular(sg->Lp);
            }
            break;
         case LW_sss:
            lw = AiLightGetSSS(sg->Lp);
            break;
         case LW_none:
            lw = 1.0f;
         default:
            break;
         }
      }
      
      if (lw <= 0.0f)
      {
         sg->out.RGB = AI_RGB_BLACK;
         return;
      }
      
      float cw = (data->surface_color_weight_is_linked ? AiShaderEvalParamFlt(p_surface_color_weight) : data->surface_color_weight);
      
      if (cw <= 0.0f)
      {
         sg->out.RGB = AI_RGB_BLACK;
         return;
      }
      
      AtColor c0 = AiEvaluateLightSample(sg, brdf->data, brdf->evalSample, brdf->evalBrdf, brdf->evalPdf);
      
      if (AiColorIsZero(c0))
      {
         sg->out.RGB = AI_RGB_BLACK;
         return;
      }
      
      AtColor c1;
      if (data->surface_color_is_linked)
      {
         c1 = AiShaderEvalParamRGB(p_surface_color);
         if (AiColorIsZero(c1))
         {
            sg->out.RGB = AI_RGB_BLACK;
            return;
         }
      }
      else
      {
         c1 = data->surface_color;
      }
      
      sg->out.RGB = lw * cw * c0 * c1;
   }
}
