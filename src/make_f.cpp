#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MakeFMtd);

enum MakeFParams
{
   p_value = 0
};

node_parameters
{
   AiParameterFlt("value", 0.0f);
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
   sg->out.FLT = AiShaderEvalParamFlt(p_value);
}
