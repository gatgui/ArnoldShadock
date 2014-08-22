#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GaussianC3Mtd);

enum GaussianC3Params
{
   p_input = 0,
   p_amplitude,
   p_center,
   p_width, // standard devisation
   p_offset
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("amplitude", 1.0f);
   AiParameterFlt("center", 0.0f);
   AiParameterFlt("width", 1.0f);
   AiParameterFlt("offset", 0.0f);
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
   AtColor input = AiShaderEvalParamRGB(p_input);
   float amplitude = AiShaderEvalParamFlt(p_amplitude);
   float center = AiShaderEvalParamFlt(p_center);
   float width = AiShaderEvalParamFlt(p_width);
   float offset = AiShaderEvalParamFlt(p_offset);
   
   float tmpr = input.r - center;
   float tmpg = input.g - center;
   float tmpb = input.b - center;
   
   float scl = 1.0f / (2.0f * width * width);
   
   sg->out.RGB.r = offset + amplitude * expf(-(tmpr * tmpr) * scl);
   sg->out.RGB.g = offset + amplitude * expf(-(tmpg * tmpg) * scl);
   sg->out.RGB.b = offset + amplitude * expf(-(tmpb * tmpb) * scl);
}
