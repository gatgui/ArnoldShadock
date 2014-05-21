#include <ai.h>

AI_SHADER_NODE_EXPORT_METHODS(SelectFloatMtd);

enum SelectFloatMtd
{
   p_condition = 0,
   p_if_true,
   p_if_false
};

node_parameters
{
   AiParameterBool("condition", true);
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
   if (AiShaderEvalParamBool(p_condition))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_if_true);
   }
   else
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_if_false);
   }
}
