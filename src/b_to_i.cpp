#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BToIMtd);

enum BToIParams
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
   sg->out.INT = (AiShaderEvalParamBool(p_input) ? 1 : 0);
}
