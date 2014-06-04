#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(AbsVectorMtd);

enum AbsVectorParams
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
   
   sg->out.VEC.x = ABS(input.x);
   sg->out.VEC.y = ABS(input.y);
   sg->out.VEC.z = ABS(input.z);
}
