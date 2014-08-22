#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GainFMtd);

enum GainFParams
{
   p_input = 0,
   p_gain
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterFlt("gain", 0.5f);
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
   float gain = AiShaderEvalParamFlt(p_gain);
   
   sg->out.FLT = GAIN(input, gain);
}
