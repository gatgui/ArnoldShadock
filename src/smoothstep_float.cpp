#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SmoothStepFloatMtd);

enum SmoothStepFloatParams
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
   
   // remap input
   float x = (input - input_min) / (input_max - input_min);
   
   // clamp to [0, 1] range
   x = CLAMP(x, 0.0f, 1.0f);
   
   // 3t^2 - 2t^3
   //x = x * x * (3.0f - 2.0f * x);
   
   // 6t^5 - 15t^4 + 10t^3 (Perlin)
   x = x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
   
   // remap output
   sg->out.FLT = output_min + x * (output_max - output_min);
}
