#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RangeSwitchVMtd);

enum RangeSwitchVParams
{
   p_value = 0,
   p_lower_bound,
   p_upper_bound,
   p_inside_range,
   p_outside_range
};

node_parameters
{
   AiParameterFlt("value", 1.0f);
   AiParameterFlt("lower_bound", 1.0f);
   AiParameterFlt("upper_bound", 1.0f);
   AiParameterVec("inside_range", 1.0f, 1.0f, 1.0f);
   AiParameterVec("outside_range", 0.0f, 0.0f, 0.0f);
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
   float value = AiShaderEvalParamFlt(p_value);
   float lower = AiShaderEvalParamFlt(p_lower_bound);
   float upper = AiShaderEvalParamFlt(p_upper_bound);
   
   if (lower <= value && value <= upper)
   {
      sg->out.VEC = AiShaderEvalParamVec(p_inside_range);
   }
   else
   {
      sg->out.VEC = AiShaderEvalParamVec(p_outside_range);
   }
}
