#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SmoothRangeSelectVectorMtd);

enum SmoothRangeSelectVectorParams
{
   p_value = 0,
   p_lower_bound,
   p_upper_bound,
   p_falloff,
   p_inside_range,
   p_outside_range
};

node_parameters
{
   AiParameterFlt("value", 1.0f);
   AiParameterFlt("lower_bound", 1.0f);
   AiParameterFlt("upper_bound", 1.0f);
   AiParameterFlt("falloff", 0.0f);
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
   float falloff = AiShaderEvalParamFlt(p_falloff);
   
   if (lower <= value && value <= upper)
   {
      sg->out.VEC = AiShaderEvalParamVec(p_inside_range);
   }
   else if (value < lower-falloff || value > upper+falloff)
   {
      sg->out.VEC = AiShaderEvalParamVec(p_outside_range);
   }
   else
   {
      AtVector insideValue = AiShaderEvalParamVec(p_inside_range);
      AtVector outsideValue = AiShaderEvalParamVec(p_outside_range);
      float blend;
      
      if (value <= lower)
      {
         blend = SmoothStep(NormalizeToRange(value, lower-falloff, lower));
      }
      else
      {
         blend = 1.0f - SmoothStep(NormalizeToRange(value, upper, upper+falloff));
      }
      
      sg->out.VEC = (1.0f - blend) * outsideValue + blend * insideValue;
   }
}
