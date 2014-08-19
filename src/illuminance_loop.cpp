#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(IlluminanceLoopMtd);

enum IlluminanceLoopParams
{
   p_input = 0,
   p_combine,
   p_reset_lights_cache
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("combine", CM_Add, CombineModeNames);
   AiParameterBool("reset_lights_cache", false);
   
   AiMetaDataSetBool(mds, "combine", "linkable", false);
   AiMetaDataSetBool(mds, "reset_lights_cache", "linkable", false);
}

struct IlluminanceLoopData
{
   CombineMode combine;
   bool reset_lights_cache;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(IlluminanceLoopData)));
}

node_update
{
   IlluminanceLoopData *data = (IlluminanceLoopData*) AiNodeGetLocalData(node);
   
   data->combine = (CombineMode) AiNodeGetInt(node, "combine");
   data->reset_lights_cache = AiNodeGetBool(node, "reset_lights_cache");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   IlluminanceLoopData *data = (IlluminanceLoopData*) AiNodeGetLocalData(node);
   
   if (data->reset_lights_cache)
   {
      AiLightsResetCache(sg);
   }
   
   AiLightsPrepare(sg);
   
   sg->out.RGB = AI_RGB_BLACK;
   
   float sample_count = 0.0f;
   AtColor sample_value;
   
   while (AiLightsGetSample(sg))
   {
      sample_value = AiShaderEvalParamRGB(p_input);
      switch (data->combine)
      {
      case CM_Max:
         sg->out.RGB.r = MAX(sample_value.r, sg->out.RGB.r);
         sg->out.RGB.g = MAX(sample_value.g, sg->out.RGB.g);
         sg->out.RGB.b = MAX(sample_value.b, sg->out.RGB.b);
         break;
      case CM_Min:
         sg->out.RGB.r = MIN(sample_value.r, sg->out.RGB.r);
         sg->out.RGB.g = MIN(sample_value.g, sg->out.RGB.g);
         sg->out.RGB.b = MIN(sample_value.b, sg->out.RGB.b);
         break;
      case CM_Add:
      case CM_Avg:
      default:
         sg->out.RGB += sample_value;
      }
      
      sample_count += 1.0f;
   }
   
   if (data->combine == CM_Avg && sample_count > 0.0f)
   {
      sample_count = 1.0f / sample_count;
      sg->out.RGB *= sample_count;
   }
}
