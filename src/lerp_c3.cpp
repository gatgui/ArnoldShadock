#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(LerpC3Mtd);

enum LerpC3Params
{
   p_input1 = 0,
   p_input2,
   p_blend
};

node_parameters
{
   AiParameterRGB("input1", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input2", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("blend", 0.0f);
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
   AtColor c1 = AiShaderEvalParamRGB(p_input1);
   AtColor c2 = AiShaderEvalParamRGB(p_input2);
   float blend = AiShaderEvalParamFlt(p_blend);
   
   sg->out.RGB = AiColorLerp(blend, c1, c2);
}
