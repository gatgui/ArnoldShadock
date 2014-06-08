#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FloatToRGBAMtd);

enum FloatToRGBAParams
{
   p_input = 0,
   p_alpha
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterFlt("alpha", 1.0f);
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
   float alpha = AiShaderEvalParamFlt(p_alpha);
   
   sg->out.RGBA.r = input;
   sg->out.RGBA.g = input;
   sg->out.RGBA.b = input;
   sg->out.RGBA.a = alpha;
}
