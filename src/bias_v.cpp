#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BiasVMtd);

enum BiasVParams
{
   p_input = 0,
   p_bias
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterVec("bias", 0.5f, 0.5f, 0.5f);
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
   
   sg->out.VEC.x = BIAS(input.x, bias.x);
   sg->out.VEC.y = BIAS(input.y, bias.y);
   sg->out.VEC.z = BIAS(input.z, bias.z);
}
