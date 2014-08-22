#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MixC3Mtd);

enum MixC3Params
{
   p_mix = 0,
   p_input1,
   p_input2
};

node_parameters
{
   AiParameterFlt("mix", 0.5f);
   AiParameterRGB("input1", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input2", 0.0f, 0.0f, 0.0f);
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
   float mix = AiShaderEvalParamFlt(p_mix);
   AtRGB input1 = AiShaderEvalParamRGB(p_input1);
   AtRGB input2 = AiShaderEvalParamRGB(p_input2);
   
   sg->out.RGB = AiColorLerp(mix, input1, input2);
}
