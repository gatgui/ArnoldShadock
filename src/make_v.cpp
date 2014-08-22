#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MakeVMtd);

enum MakeVParams
{
   p_x = 0,
   p_y,
   p_z
};

node_parameters
{
   AiParameterFlt("x", 0.0f);
   AiParameterFlt("y", 0.0f);
   AiParameterFlt("z", 0.0f);
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
   sg->out.VEC.x = AiShaderEvalParamFlt(p_x);
   sg->out.VEC.y = AiShaderEvalParamFlt(p_y);
   sg->out.VEC.z = AiShaderEvalParamFlt(p_z);
}
