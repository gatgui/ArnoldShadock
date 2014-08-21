#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GainColorMtd);

enum GainColorParams
{
   p_input = 0,
   p_gain
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterVec("gain", 0.5f, 0.5f, 0.5f);
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
   AtVector gain = AiShaderEvalParamVec(p_gain);
   
   sg->out.RGB.r = GAIN(input.r, gain.x);
   sg->out.RGB.g = GAIN(input.g, gain.y);
   sg->out.RGB.b = GAIN(input.b, gain.z);
}
