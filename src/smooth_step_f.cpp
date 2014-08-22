#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SmoothStepFMtd);

enum SmoothStepFParams
{
   p_input = 0,
   p_input_min,
   p_input_max,
   p_output_min,
   p_output_max
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterFlt("input_min", 0.0f);
   AiParameterFlt("input_max", 1.0f);
   AiParameterFlt("output_min", 0.0f);
   AiParameterFlt("output_max", 1.0f);
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
   float output_min = AiShaderEvalParamFlt(p_output_min);
   float output_max = AiShaderEvalParamFlt(p_output_max);
   
   sg->out.FLT = ExpandToRange(SmoothStep(NormalizeToRange(input, input_min, input_max)), output_min, output_max);
}
