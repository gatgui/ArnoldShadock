#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FloorColorMtd);

enum FloorColorParams
{
   p_input
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
   
   sg->out.RGB.r = FLOOR(input.r);
   sg->out.RGB.g = FLOOR(input.g);
   sg->out.RGB.b = FLOOR(input.b);
}
