#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SmoothSwitchFMtd);

enum SmoothSwitchFParams
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
   AiParameterFlt("if_true", 1.0f);
   AiParameterFlt("if_false", 0.0f);
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
      sg->out.FLT = AiShaderEvalParamFlt(p_if_true);
   }
   else if (value < threshold-falloff)
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_if_false);
   }
   else
   {
      float blend = SmoothStep(NormalizeToRange(value, threshold-falloff, threshold+falloff));
      
      float trueValue = AiShaderEvalParamFlt(p_if_true);
      float falseValue = AiShaderEvalParamFlt(p_if_false);
      
      sg->out.FLT = (1.0f - blend) * falseValue + blend * trueValue;
   }
}
