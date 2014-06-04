#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShadingPointMtd);

enum ShadingPointParams
{
   p_time = 0
};

node_parameters
{
   AiParameterFlt("time", 0.0f);
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
   AiShaderGlobalsGetPositionAtTime(sg, AiShaderEvalParamFlt(p_time), &(sg->out.PNT), 0, 0, 0);
}
