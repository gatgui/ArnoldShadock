#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(LogVMtd);

enum LogVParams
{
   p_input = 0
};

node_parameters
{
   AiParameterVec("input", 1.0f, 1.0f, 1.0f);
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
   
   sg->out.VEC.x = logf(input.x);
   sg->out.VEC.y = logf(input.y);
   sg->out.VEC.z = logf(input.z);
}
