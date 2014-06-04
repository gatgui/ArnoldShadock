#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(PowFloatMtd);

enum PowFloatParams
{
   p_input1 = 0,
   p_input2
};

node_parameters
{
   AiParameterFlt("input1", 0.0f);
   AiParameterFlt("input2", 1.0f);
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
   float input1 = AiShaderEvalParamFlt(p_input1);
   float input2 = AiShaderEvalParamFlt(p_input2);
   
   sg->out.FLT = powf(input1, input2);
}
