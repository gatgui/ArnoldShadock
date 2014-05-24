#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RoundVectorMtd);

enum RoundVectorParams
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
   
   sg->out.VEC.x = ROUND(input.x);
   sg->out.VEC.y = ROUND(input.y);
   sg->out.VEC.z = ROUND(input.z);
}
