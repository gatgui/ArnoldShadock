#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MultiSwitchC3Mtd);

enum MultiSwitchC3Params
{
   p_which = 0,
   p_input0,
   p_input1,
   p_input2,
   p_input3,
   p_input4,
   p_input5,
   p_input6,
   p_input7,
   p_input8,
   p_input9,
   p_input10,
   p_input11,
   p_input12,
   p_input13,
   p_input14,
   p_input15,
};

node_parameters
{
   AiParameterInt("which", 0);
   AiParameterRGB("input0", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input1", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input2", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input3", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input4", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input5", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input6", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input7", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input8", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input9", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input10", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input11", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input12", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input13", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input14", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input15", 0.0f, 0.0f, 0.0f);
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
   int which = AiShaderEvalParamInt(p_which);
   sg->out.RGB = AiShaderEvalParamRGB(p_input0 + (which < 0 ? 0 : (which > 15 ? 15 : which)));
}
