#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GaussianVectorMtd);

enum GaussianVectorParams
{
   p_input = 0,
   p_amplitude,
   p_center,
   p_width, // standard devisation
   p_offset
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
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
   AtVector input = AiShaderEvalParamVec(p_input);
   float amplitude = AiShaderEvalParamFlt(p_amplitude);
   float center = AiShaderEvalParamFlt(p_center);
   float width = AiShaderEvalParamFlt(p_width);
   float offset = AiShaderEvalParamFlt(p_offset);
   
   float tmpx = input.x - center;
   float tmpy = input.y - center;
   float tmpz = input.z - center;
   
   float scl = 1.0f / (2.0f * width * width);
   
   sg->out.VEC.x = offset + amplitude * expf(-(tmpx * tmpx) * scl);
   sg->out.VEC.y = offset + amplitude * expf(-(tmpy * tmpy) * scl);
   sg->out.VEC.z = offset + amplitude * expf(-(tmpz * tmpz) * scl);
}
