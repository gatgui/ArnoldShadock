#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ConvertColorToVectorMtd);

enum ConvertColorToVectorParams
{
   p_input = 0
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
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
   AtRGB input = AiShaderEvalParamRGB(p_input);
   
   sg->out.VEC.x = input.r;
   sg->out.VEC.y = input.g;
   sg->out.VEC.z = input.b;
}
