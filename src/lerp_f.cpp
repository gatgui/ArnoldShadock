#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(LerpFMtd);

enum LerpFParams
{
   p_input1 = 0,
   p_input2,
   p_blend
};

node_parameters
{
   AiParameterFlt("input1", 0.0f);
   AiParameterFlt("input2", 0.0f);
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
   float f1 = AiShaderEvalParamFlt(p_input1);
   float f2 = AiShaderEvalParamFlt(p_input2);
   float blend = AiShaderEvalParamFlt(p_blend);
   
   sg->out.FLT = (1.0f - blend) * f1 + blend * f2;
}
