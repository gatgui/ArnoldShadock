#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(EvalLightSampleMtd);

enum EvalLightSampleParams
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
   AiParameterRGB(SSTR::surface_color, 1.0f, 1.0f, 1.0f);
   AiParameterFlt(SSTR::surface_color_weight, 1.0f);
   AiParameterEnum(SSTR::light_weight, LW_none, LightWeightNames);
}

struct NodeData
{
   bool evalSurfaceColor;
   AtColor surfaceColor;
   
   bool evalSurfaceColorWeight;
   float surfaceColorWeight;
   
   LightWeight lightWeight;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->evalSurfaceColor = AiNodeIsLinked(node, SSTR::surface_color);
   if (!data->evalSurfaceColor)
   {
      data->surfaceColor = AiNodeGetRGB(node, SSTR::surface_color);
   }
   
   data->evalSurfaceColorWeight = AiNodeIsLinked(node, SSTR::surface_color_weight);
   if (!data->evalSurfaceColorWeight)
   {
      data->surfaceColorWeight = AiNodeGetFlt(node, SSTR::surface_color_weight);
   }
   
   data->lightWeight = (LightWeight) AiNodeGetInt(node, SSTR::light_weight);
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   BRDFData *brdf = 0;
   
   AiStateSetMsgPtr(SSTR::agsb_brdf, 0);
   
   AiShaderEvalParamRGB(p_brdf);
   
   if (!AiStateGetMsgPtr(SSTR::agsb_brdf, (void**)&brdf) || !brdf)
   {
      sg->out.RGB = AI_RGB_BLACK;
   }
   else
   {
      if (!data->evalSurfaceColor && AiColorIsZero(data->surfaceColor))
      {
         sg->out.RGB = AI_RGB_BLACK;
         return;
      }
      
      if (!data->evalSurfaceColorWeight && data->surfaceColorWeight <= 0.0f)
      {
         sg->out.RGB = AI_RGB_BLACK;
         return;
      }
      
      float lw = 0.0f;
      
      if (sg->Lp)
      {
         switch (data->lightWeight)
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
      
      float cw = (data->evalSurfaceColorWeight ? AiShaderEvalParamFlt(p_surface_color_weight) : data->surfaceColorWeight);
      
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
      if (data->evalSurfaceColor)
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
         c1 = data->surfaceColor;
      }
      
      sg->out.RGB = lw * cw * c0 * c1;
   }
}
