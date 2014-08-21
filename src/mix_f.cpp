#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MixFloatMtd);

enum MixFloatParams
{
   p_mix = 0,
   p_input1,
   p_input2
};

node_parameters
{
   AiParameterFlt("mix", 0.5f);
   AiParameterFlt("input1", 0.0f);
   AiParameterFlt("input2", 0.0f);
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
   float input1 = AiShaderEvalParamFlt(p_input1);
   float input2 = AiShaderEvalParamFlt(p_input2);
   
   sg->out.FLT = input1 + mix * (input2 - input1);
}
