#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ClampFMtd);

enum ClampFParams
{
   p_input = 0,
   p_input_min,
   p_input_max
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterFlt("input_min", 0.0f);
   AiParameterFlt("input_max", 1.0f);
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
   float input_min = AiShaderEvalParamFlt(p_input_min);
   float input_max = AiShaderEvalParamFlt(p_input_max);
   
   sg->out.FLT = CLAMP(input, input_min, input_max);
}
