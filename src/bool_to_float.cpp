#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BoolToFloatMtd);

enum BoolToFloatParams
{
   p_input = 0
};

node_parameters
{
   AiParameterBool("input", false);
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
   sg->out.FLT = (AiShaderEvalParamBool(p_input) ? 1.0f : 0.0f);
}
