#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MixVMtd);

enum MixVParams
{
   p_mix = 0,
   p_input1,
   p_input2
};

node_parameters
{
   AiParameterFlt("mix", 0.5f);
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 0.0f, 0.0f, 0.0f);
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
   AtVector input1 = AiShaderEvalParamVec(p_input1);
   AtVector input2 = AiShaderEvalParamVec(p_input2);
   
   sg->out.VEC = AiV3Lerp(mix, input1, input2);
}
