#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GainVectorMtd);

enum GainVectorParams
{
   p_input,
   p_gain
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
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
   float gain = AiShaderEvalParamFlt(p_gain);
   
   sg->out.VEC.x = GAIN(input.x, gain);
   sg->out.VEC.y = GAIN(input.y, gain);
   sg->out.VEC.z = GAIN(input.z, gain);
}
