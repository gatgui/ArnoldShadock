#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MinFloatMtd);

enum MinFloatParams
{
   p_input1,
   p_input2
};

node_parameters
{
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
   float input1 = AiShaderEvalParamFlt(p_input1);
   float input2 = AiShaderEvalParamFlt(p_input2);
   
   sg->out.FLT = MIN(input1, input2);
}
