#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ConvertVectorToColorMtd);

enum ConvertVectorToColorParams
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
   
   sg->out.RGB.r = input.x;
   sg->out.RGB.g = input.y;
   sg->out.RGB.b = input.z;
}
