#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(C3ToC4Mtd);

enum C3ToC4Params
{
   p_color = 0,
   p_alpha
};

node_parameters
{
   AiParameterRGB("color", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("alpha", 1.0f);
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
   AtRGB input = AiShaderEvalParamRGB(p_color);

   sg->out.RGBA.r = input.r;
   sg->out.RGBA.g = input.g;
   sg->out.RGBA.b = input.b;
   sg->out.RGBA.a = AiShaderEvalParamFlt(p_alpha);
}
