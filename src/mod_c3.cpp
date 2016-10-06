#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ModC3Mtd);

enum ModC3Params
{
   p_input1 = 0,
   p_input2
};

node_parameters
{
   AiParameterRGB("input1", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input2", 1.0f, 1.0f, 1.0f);
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
   AtRGB input1 = AiShaderEvalParamRGB(p_input1);
   AtRGB input2 = AiShaderEvalParamRGB(p_input2);
   
   sg->out.RGB.r = fmodf(input1.r, input2.r);
   sg->out.RGB.g = fmodf(input1.g, input2.g);
   sg->out.RGB.b = fmodf(input1.b, input2.b);
}
