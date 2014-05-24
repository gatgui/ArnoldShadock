#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(InverseVectorMtd);

enum InverseVectorParams
{
   p_input
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
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
   
   sg->out.VEC.x = 1.0f / input.x;
   sg->out.VEC.y = 1.0f / input.y;
   sg->out.VEC.z = 1.0f / input.z;
}
