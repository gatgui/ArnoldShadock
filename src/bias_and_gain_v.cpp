#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BiasAndGainVMtd);

enum BiasAndGainVParams
{
   p_input = 0,
   p_bias,
   p_gain
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
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
   AtVector input = AiShaderEvalParamVec(p_input);
   AtVector bias = AiShaderEvalParamVec(p_bias);
   AtVector gain = AiShaderEvalParamVec(p_gain);
   
   sg->out.VEC.x = GAIN(BIAS(input.x, bias.x), gain.x);
   sg->out.VEC.y = GAIN(BIAS(input.y, bias.y), gain.y);
   sg->out.VEC.z = GAIN(BIAS(input.z, bias.z), gain.z);
}
