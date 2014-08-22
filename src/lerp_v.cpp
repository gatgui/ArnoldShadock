#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(LerpVMtd);

enum LerpVParams
{
   p_input1 = 0,
   p_input2,
   p_blend
};

node_parameters
{
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 0.0f, 0.0f, 0.0f);
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
   AtVector v1 = AiShaderEvalParamVec(p_input1);
   AtVector v2 = AiShaderEvalParamVec(p_input2);
   float blend = AiShaderEvalParamFlt(p_blend);
   
   sg->out.VEC = AiV3Lerp(blend, v1, v2);
}
