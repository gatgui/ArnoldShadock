#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BiasAndGainFloatMtd);

enum BiasAndGainFloatParams
{
   p_input,
   p_bias,
   p_gain
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
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
   float input = AiShaderEvalParamFlt(p_input);
   float bias = AiShaderEvalParamFlt(p_bias);
   float gain = AiShaderEvalParamFlt(p_gain);
   
   sg->out.FLT = GAIN(BIAS(input, bias), gain);
}
