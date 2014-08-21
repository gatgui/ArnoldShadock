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
   
   sg->out.VEC = ExpandToRange(SmoothStep(NormalizeToRange(input, input_min, input_max)), output_min, output_max);
}
