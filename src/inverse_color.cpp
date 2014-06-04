#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(InverseColorMtd);

enum InverseColorParams
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
   
   sg->out.RGB.r = 1.0f / input.r;
   sg->out.RGB.g = 1.0f / input.g;
   sg->out.RGB.b = 1.0f / input.b;
}
