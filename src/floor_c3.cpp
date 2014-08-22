#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FloorC3Mtd);

enum FloorC3Params
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
   
   sg->out.RGB.r = float(FLOOR(input.r));
   sg->out.RGB.g = float(FLOOR(input.g));
   sg->out.RGB.b = float(FLOOR(input.b));
}
