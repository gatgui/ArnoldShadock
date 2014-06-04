#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BiasFloatMtd);

enum BiasFloatParams
{
   p_input = 0,
   p_bias
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterFlt("bias", 0.5f);
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
   float input = AiShaderEvalParamFlt(p_input);
   float bias = AiShaderEvalParamFlt(p_bias);
   
   sg->out.FLT = BIAS(input, bias);
}
