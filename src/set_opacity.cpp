#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SetOpacityMtd);

enum SetOpacityParams
{
   p_color = 0,
   p_opacity
};

node_parameters
{
   AiParameterRGB("color", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("opacity", 1.0f, 1.0f, 1.0f);
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
   AtRGB opacity = AiShaderEvalParamRGB(p_opacity);
   
   if (MaxComponent<3>(&(opacity.r)) <= AI_EPSILON)
   {
      sg->out.RGB = AI_RGB_BLACK;
      sg->out_opacity = AI_RGB_BLACK;
   }
   else
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_color);
      sg->out_opacity = opacity;
   }
}
