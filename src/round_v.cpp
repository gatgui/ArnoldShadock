#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RoundVMtd);

enum RoundVParams
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
   
   sg->out.VEC.x = float(ROUND(input.x));
   sg->out.VEC.y = float(ROUND(input.y));
   sg->out.VEC.z = float(ROUND(input.z));
}
