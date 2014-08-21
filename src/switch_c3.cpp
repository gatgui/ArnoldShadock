#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SelectColorMtd);

enum SelectColorParams
{
   p_condition = 0,
   p_if_true,
   p_if_false
};

node_parameters
{
   AiParameterBool("condition", true);
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
   if (AiShaderEvalParamBool(p_condition))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_if_true);
   }
   else
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_if_false);
   }
}
