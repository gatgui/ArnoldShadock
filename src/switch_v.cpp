#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SwitchVMtd);

enum SwitchVParams
{
   p_condition = 0,
   p_if_true,
   p_if_false
};

node_parameters
{
   AiParameterBool("condition", true);
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
   if (AiShaderEvalParamBool(p_condition))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_if_true);
   }
   else
   {
      sg->out.VEC = AiShaderEvalParamVec(p_if_false);
   }
}
