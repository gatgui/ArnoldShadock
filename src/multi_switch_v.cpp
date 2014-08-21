#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MultiSelectVectorMtd);

enum MultiSelectVectorParams
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
   AiParameterVec("input0", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input3", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input4", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input5", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input6", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input7", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input8", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input9", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input10", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input11", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input12", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input13", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input14", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input15", 0.0f, 0.0f, 0.0f);
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
   sg->out.VEC = AiShaderEvalParamVec(p_input0 + (which < 0 ? 0 : (which > 15 ? 15 : which)));
}
