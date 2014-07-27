#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(IlluminanceLoopMtd);

enum IlluminanceLoopParams
{
   p_input = 0,
   p_reset_lights_cache
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterBool("reset_lights_cache", false);
   
   AiMetaDataSetBool(mds, "reset_lights_cache", "linkable", false);
}

node_initialize
{
}

node_update
{
   AiNodeSetLocalData(node, (void*)(AiNodeGetBool(node, "reset_lights_cache") ? 1 : 0));
}

node_finish
{
}

shader_evaluate
{
   bool reset_lights_cache = (AiNodeGetLocalData(node) != 0);
   
   sg->out.RGB = AI_RGB_BLACK;
   
   if (reset_lights_cache)
   {
      AiLightsResetCache(sg);
   }
   
   AiLightsPrepare(sg);
   
   while (AiLightsGetSample(sg))
   {
      sg->out.RGB += AiShaderEvalParamRGB(p_input);
   }
}
