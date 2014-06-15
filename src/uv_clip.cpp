#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVClipMtd);

enum UVClipParams
{
   p_input = 0,
   p_clipped_input,
   p_u_range,
   p_v_range
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("clipped_input", 0.0f, 0.0f, 0.0f);
   AiParameterPnt2("u_range", 0.0f, 1.0f);
   AiParameterPnt2("v_range", 0.0f, 1.0f);
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
   AtPoint2 u_range = AiShaderEvalParamPnt2(p_u_range);
   AtPoint2 v_range = AiShaderEvalParamPnt2(p_v_range);
   
   if (sg->u < u_range.x || sg->u > u_range.y ||
       sg->v < v_range.x || sg->v > v_range.y)
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_clipped_input);
   }
   else
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_input);
   }
}
