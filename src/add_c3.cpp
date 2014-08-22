#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(AddC3Mtd);

enum AddColorParams
{
   p_input1 = 0,
   p_input2
};

node_parameters
{
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
   AtRGB input1 = AiShaderEvalParamRGB(p_input1);
   AtRGB input2 = AiShaderEvalParamRGB(p_input2);
   
   sg->out.RGB = input1 + input2;
}
