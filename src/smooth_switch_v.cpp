#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SmoothSwitchVMtd);

enum SmoothSwitchVParams
{
   p_value = 0,
   p_threshold,
   p_falloff,
   p_if_true,
   p_if_false
};

node_parameters
{
   AiParameterFlt("value", 1.0f);
   AiParameterFlt("threshold", 1.0f);
   AiParameterFlt("falloff", 0.0f);
   AiParameterVec("if_true", 1.0f, 1.0f, 1.0f);
   AiParameterVec("if_false", 0.0f, 0.0f, 0.0f);
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
   float threshold = AiShaderEvalParamFlt(p_threshold);
   float falloff = fabs(AiShaderEvalParamFlt(p_falloff));
   
   if (value >= threshold+falloff)
   {
      sg->out.VEC = AiShaderEvalParamVec(p_if_true);
   }
   else if (value < threshold-falloff)
   {
      sg->out.VEC = AiShaderEvalParamVec(p_if_false);
   }
   else
   {
      float blend = SmoothStep(NormalizeToRange(value, threshold-falloff, threshold+falloff));
      
      AtVector trueValue = AiShaderEvalParamVec(p_if_true);
      AtVector falseValue = AiShaderEvalParamVec(p_if_false);
      
      sg->out.VEC = (1.0f - blend) * falseValue + blend * trueValue;
   }
}
