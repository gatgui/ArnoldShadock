#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FloatToBoolMtd);

enum FloatToBoolParams
{
   p_input = 0,
   p_threshold
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterFlt("threshold", 1.0f);
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
   sg->out.BOOL = (AiShaderEvalParamFlt(p_input) >= AiShaderEvalParamFlt(p_threshold));
}
