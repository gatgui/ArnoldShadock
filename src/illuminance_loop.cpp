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
   AiParameterEnum(SSTR::combine, CM_Add, CombineModeNames);
   AiParameterBool(SSTR::reset_lights_cache, false);
}

struct NodeData
{
   CombineMode combine;
   bool resetLightsCache;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->combine = (CombineMode) AiNodeGetInt(node, SSTR::combine);
   data->resetLightsCache = AiNodeGetBool(node, SSTR::reset_lights_cache);
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
   
   if (data->resetLightsCache)
   {
      AiLightsResetCache(sg);
   }
   
   AiLightsPrepare(sg);
   
   sg->out.RGB = AI_RGB_BLACK;
   
   float sampleCount = 0.0f;
   AtColor sampleValue;
   
   while (AiLightsGetSample(sg))
   {
      sampleValue = AiShaderEvalParamRGB(p_input);
      switch (data->combine)
      {
      case CM_Max:
         sg->out.RGB.r = MAX(sampleValue.r, sg->out.RGB.r);
         sg->out.RGB.g = MAX(sampleValue.g, sg->out.RGB.g);
         sg->out.RGB.b = MAX(sampleValue.b, sg->out.RGB.b);
         break;
      case CM_Min:
         sg->out.RGB.r = MIN(sampleValue.r, sg->out.RGB.r);
         sg->out.RGB.g = MIN(sampleValue.g, sg->out.RGB.g);
         sg->out.RGB.b = MIN(sampleValue.b, sg->out.RGB.b);
         break;
      case CM_Add:
      case CM_Avg:
      default:
         sg->out.RGB += sampleValue;
      }
      
      sampleCount += 1.0f;
   }
   
   if (data->combine == CM_Avg && sampleCount > 0.0f)
   {
      sampleCount = 1.0f / sampleCount;
      sg->out.RGB *= sampleCount;
   }
}
