#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RoundColorMtd);

enum RoundColorParams
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
   
   sg->out.RGB.r = ROUND(input.r);
   sg->out.RGB.g = ROUND(input.g);
   sg->out.RGB.b = ROUND(input.b);
}
