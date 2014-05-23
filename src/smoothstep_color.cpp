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
   
   // remap input
   float r = (input.r - input_min.r) / (input_max.r - input_min.r);
   float g = (input.g - input_min.g) / (input_max.g - input_min.g);
   float b = (input.b - input_min.b) / (input_max.b - input_min.b);
   
   // clamp to [0, 1] range
   r = CLAMP(r, 0.0f, 1.0f);
   g = CLAMP(g, 0.0f, 1.0f);
   b = CLAMP(b, 0.0f, 1.0f);
   
   // 3t^2 - 2t^3
   //r = r * r * (3.0f - 2.0f * r);
   //g = g * g * (3.0f - 2.0f * g);
   //b = b * b * (3.0f - 2.0f * b);
   
   // 6t^5 - 15t^4 + 10t^3 (Perlin)
   r = r * r * r * (r * (r * 6.0f - 15.0f) + 10.0f);
   g = g * g * g * (g * (g * 6.0f - 15.0f) + 10.0f);
   b = b * b * b * (b * (b * 6.0f - 15.0f) + 10.0f);
   
   // remap output
   sg->out.RGB.r = output_min.r + r * (output_max.r - output_min.r);
   sg->out.RGB.g = output_min.g + g * (output_max.g - output_min.g);
   sg->out.RGB.b = output_min.b + b * (output_max.b - output_min.b);
}
