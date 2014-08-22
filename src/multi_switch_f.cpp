#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MultiSwitchFMtd);

enum MultiSwitchFParams
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
   AiParameterFlt("input0", 0.0f);
   AiParameterFlt("input1", 0.0f);
   AiParameterFlt("input2", 0.0f);
   AiParameterFlt("input3", 0.0f);
   AiParameterFlt("input4", 0.0f);
   AiParameterFlt("input5", 0.0f);
   AiParameterFlt("input6", 0.0f);
   AiParameterFlt("input7", 0.0f);
   AiParameterFlt("input8", 0.0f);
   AiParameterFlt("input9", 0.0f);
   AiParameterFlt("input10", 0.0f);
   AiParameterFlt("input11", 0.0f);
   AiParameterFlt("input12", 0.0f);
   AiParameterFlt("input13", 0.0f);
   AiParameterFlt("input14", 0.0f);
   AiParameterFlt("input15", 0.0f);
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
   sg->out.FLT = AiShaderEvalParamFlt(p_input0 + (which < 0 ? 0 : (which > 15 ? 15 : which)));
}
