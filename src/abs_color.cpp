#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(AbsColorMtd);

enum AbsColorParams
{
   p_input = 0
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
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
   AtRGB input = AiShaderEvalParamRGB(p_input);
   
   sg->out.RGB.r = ABS(input.r);
   sg->out.RGB.g = ABS(input.g);
   sg->out.RGB.b = ABS(input.b);
}
