#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(LogC3Mtd);

enum LogC3Params
{
   p_input = 0
};

node_parameters
{
   AiParameterRGB("input", 1.0f, 1.0f, 1.0f);
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
   
   sg->out.RGB.r = logf(input.r);
   sg->out.RGB.g = logf(input.g);
   sg->out.RGB.b = logf(input.b);
}
