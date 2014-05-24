#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(NegateColorMtd);

enum NegateColorParams
{
   p_input
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
   
   sg->out.RGB = -input;
}
