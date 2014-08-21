#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BiasAndGainC3Mtd);

enum BiasAndGainC3Params
{
   p_input = 0,
   p_bias,
   p_gain
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterVec("bias", 0.5f, 0.5f, 0.5f);
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
   AtVector bias = AiShaderEvalParamVec(p_bias);
   AtVector gain = AiShaderEvalParamVec(p_gain);
   
   sg->out.RGB.r = GAIN(BIAS(input.r, bias.x), gain.x);
   sg->out.RGB.g = GAIN(BIAS(input.g, bias.y), gain.y);
   sg->out.RGB.b = GAIN(BIAS(input.b, bias.z), gain.z);
}
