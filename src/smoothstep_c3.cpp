#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SmoothStepColorMtd);

enum SmoothStepColorParams
{
   p_input = 0,
   p_input_min,
   p_input_max,
   p_output_min,
   p_output_max
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input_min", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input_max", 1.0f, 1.0f, 1.0f);
   AiParameterRGB("output_min", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("output_max", 1.0f, 1.0f, 1.0f);
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
   AtRGB input = AiShaderEvalParamRGB(p_input);
   AtRGB input_min = AiShaderEvalParamRGB(p_input_min);
   AtRGB input_max = AiShaderEvalParamRGB(p_input_max);
   AtRGB output_min = AiShaderEvalParamRGB(p_output_min);
   AtRGB output_max = AiShaderEvalParamRGB(p_output_max);
   
   sg->out.RGB = ExpandToRange(SmoothStep(NormalizeToRange(input, input_min, input_max)), output_min, output_max);
}
