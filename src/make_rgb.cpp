#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MakeRGBMtd);

enum MakeRGBParams
{
   p_r = 0,
   p_g,
   p_b
};

node_parameters
{
   AiParameterFlt("r", 0.0f);
   AiParameterFlt("g", 0.0f);
   AiParameterFlt("b", 0.0f);
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
   sg->out.RGB.r = AiShaderEvalParamFlt(p_r);
   sg->out.RGB.g = AiShaderEvalParamFlt(p_g);
   sg->out.RGB.b = AiShaderEvalParamFlt(p_b);
}
