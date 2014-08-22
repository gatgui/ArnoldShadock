#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ClampC3Mtd);

enum ClampC3Params
{
   p_input = 0,
   p_input_min,
   p_input_max
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input_min", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input_max", 1.0f, 1.0f, 1.0f);
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
   AtRGB input_min = AiShaderEvalParamRGB(p_input_min);
   AtRGB input_max = AiShaderEvalParamRGB(p_input_max);
   
   sg->out.RGB.r = CLAMP(input.r, input_min.r, input_max.r);
   sg->out.RGB.g = CLAMP(input.g, input_min.g, input_max.g);
   sg->out.RGB.b = CLAMP(input.b, input_min.b, input_max.b);
}
