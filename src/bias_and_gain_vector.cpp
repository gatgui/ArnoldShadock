#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BiasAndGainVectorMtd);

enum BiasAndGainVectorParams
{
   p_input,
   p_bias,
   p_gain
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
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
   AtVector input = AiShaderEvalParamVec(p_input);
   float bias = AiShaderEvalParamFlt(p_bias);
   float gain = AiShaderEvalParamFlt(p_gain);
   
   sg->out.VEC.x = GAIN(BIAS(input.x, bias), gain);
   sg->out.VEC.y = GAIN(BIAS(input.y, bias), gain);
   sg->out.VEC.z = GAIN(BIAS(input.z, bias), gain);
}
