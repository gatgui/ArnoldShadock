#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MakeP2Mtd);

enum MakeP2Params
{
   p_x = 0,
   p_y
};

node_parameters
{
   AiParameterFlt("x", 0.0f);
   AiParameterFlt("y", 0.0f);
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
   sg->out.PNT2.x = AiShaderEvalParamFlt(p_x);
   sg->out.PNT2.y = AiShaderEvalParamFlt(p_y);
}
