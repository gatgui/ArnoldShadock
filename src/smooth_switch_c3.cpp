#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SmoothSwitchC3Mtd);

enum SmoothSwitchC3Params
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
   AiParameterRGB("if_true", 1.0f, 1.0f, 1.0f);
   AiParameterRGB("if_false", 0.0f, 0.0f, 0.0f);
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
      sg->out.RGB = AiShaderEvalParamRGB(p_if_true);
   }
   else if (value < threshold-falloff)
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_if_false);
   }
   else
   {
      float blend = SmoothStep(NormalizeToRange(value, threshold-falloff, threshold+falloff));
      
      AtColor trueValue = AiShaderEvalParamRGB(p_if_true);
      AtColor falseValue = AiShaderEvalParamRGB(p_if_false);
      
      sg->out.RGB = (1.0f - blend) * falseValue + blend * trueValue;
   }
}
