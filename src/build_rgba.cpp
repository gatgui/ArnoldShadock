#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BuildRGBAMtd);

enum BuildRGBAParams
{
   p_r = 0,
   p_g,
   p_b,
   p_a
};

node_parameters
{
   AiParameterFlt("r", 0.0f);
   AiParameterFlt("g", 0.0f);
   AiParameterFlt("b", 0.0f);
   AiParameterFlt("a", 1.0f);
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
   sg->out.RGBA.r = AiShaderEvalParamFlt(p_r);
   sg->out.RGBA.g = AiShaderEvalParamFlt(p_g);
   sg->out.RGBA.b = AiShaderEvalParamFlt(p_b);
   sg->out.RGBA.a = AiShaderEvalParamFlt(p_a);
}
