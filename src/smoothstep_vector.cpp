#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SmoothStepVectorMtd);

enum SmoothStepVectorParams
{
   p_input = 0,
   p_input_min,
   p_input_max,
   p_output_min,
   p_output_max
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input_min", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input_max", 1.0f, 1.0f, 1.0f);
   AiParameterVec("output_min", 0.0f, 0.0f, 0.0f);
   AiParameterVec("output_max", 1.0f, 1.0f, 1.0f);
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
   AtVector input_min = AiShaderEvalParamVec(p_input_min);
   AtVector input_max = AiShaderEvalParamVec(p_input_max);
   AtVector output_min = AiShaderEvalParamVec(p_output_min);
   AtVector output_max = AiShaderEvalParamVec(p_output_max);
   
   // remap input
   float x = (input.x - input_min.x) / (input_max.x - input_min.x);
   float y = (input.y - input_min.y) / (input_max.y - input_min.y);
   float z = (input.z - input_min.z) / (input_max.z - input_min.z);
   
   // clamp to [0, 1] range
   x = CLAMP(x, 0.0f, 1.0f);
   y = CLAMP(y, 0.0f, 1.0f);
   z = CLAMP(z, 0.0f, 1.0f);
   
   // 3t^2 - 2t^3
   //x = x * x * (3.0f - 2.0f * x);
   //y = y * y * (3.0f - 2.0f * y);
   //z = z * z * (3.0f - 2.0f * z);
   
   // 6t^5 - 15t^4 + 10t^3 (Perlin)
   x = x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
   y = y * y * y * (y * (y * 6.0f - 15.0f) + 10.0f);
   z = z * z * z * (z * (z * 6.0f - 15.0f) + 10.0f);
   
   // remap output
   sg->out.VEC.x = output_min.x + x * (output_max.x - output_min.x);
   sg->out.VEC.y = output_min.y + y * (output_max.y - output_min.y);
   sg->out.VEC.z = output_min.z + z * (output_max.z - output_min.z);
}
