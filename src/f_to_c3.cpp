#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FToC3Mtd);

enum FToC3Params
{
   p_input = 0
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
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
   float input = AiShaderEvalParamFlt(p_input);
   
   sg->out.RGB.r = input;
   sg->out.RGB.g = input;
   sg->out.RGB.b = input;
}
