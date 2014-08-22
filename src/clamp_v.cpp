#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ClampVMtd);

enum ClampVParams
{
   p_input = 0,
   p_input_min,
   p_input_max
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input_min", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input_max", 1.0f, 1.0f, 1.0f);
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
   AtVector input_min = AiShaderEvalParamVec(p_input_min);
   AtVector input_max = AiShaderEvalParamVec(p_input_max);
   
   sg->out.VEC.x = CLAMP(input.x, input_min.x, input_max.x);
   sg->out.VEC.y = CLAMP(input.y, input_min.y, input_max.y);
   sg->out.VEC.z = CLAMP(input.z, input_min.z, input_max.z);
}
