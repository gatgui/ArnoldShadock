#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BiasAndGainColorMtd);

enum BiasAndGainColorParams
{
   p_input,
   p_bias,
   p_gain
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("bias", 0.5f);
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
   float bias = AiShaderEvalParamFlt(p_bias);
   float gain = AiShaderEvalParamFlt(p_gain);
   
   sg->out.RGB.r = GAIN(BIAS(input.r, bias), gain);
   sg->out.RGB.g = GAIN(BIAS(input.g, bias), gain);
   sg->out.RGB.b = GAIN(BIAS(input.b, bias), gain);
}
