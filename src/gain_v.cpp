#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GainVMtd);

enum GainVParams
{
   p_input = 0,
   p_gain
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
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
   AtVector gain = AiShaderEvalParamVec(p_gain);
   
   sg->out.VEC.x = GAIN(input.x, gain.x);
   sg->out.VEC.y = GAIN(input.y, gain.y);
   sg->out.VEC.z = GAIN(input.z, gain.z);
}
