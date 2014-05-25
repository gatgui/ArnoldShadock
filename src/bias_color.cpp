#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BiasColorMtd);

enum BiasColorParams
{
   p_input,
   p_bias
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("bias", 0.5f);
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
   
   sg->out.RGB.r = BIAS(input.r, bias);
   sg->out.RGB.g = BIAS(input.g, bias);
   sg->out.RGB.b = BIAS(input.b, bias);
}
