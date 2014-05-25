#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GainColorMtd);

enum GainColorParams
{
   p_input,
   p_gain
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("gain", 0.5f);
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
   float gain = AiShaderEvalParamFlt(p_gain);
   
   sg->out.RGB.r = GAIN(input.r, gain);
   sg->out.RGB.g = GAIN(input.g, gain);
   sg->out.RGB.b = GAIN(input.b, gain);
}
