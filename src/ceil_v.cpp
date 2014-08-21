#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(CeilVectorMtd);

enum CeilVectorParams
{
   p_input = 0
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
   
   sg->out.VEC.x = CEIL(input.x);
   sg->out.VEC.y = CEIL(input.y);
   sg->out.VEC.z = CEIL(input.z);
}
