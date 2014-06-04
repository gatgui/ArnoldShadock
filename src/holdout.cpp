#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(HoldoutMtd);

enum HoldoutParams
{
    p_input = 0,
    p_color
};

node_parameters
{
    AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
    AiParameterRGBA("color", 0.0f, 0.0f, 0.0f, 0.0f);
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
    sg->out.RGBA = ((sg->Rt & AI_RAY_CAMERA) ? AiShaderEvalParamRGBA(p_color) : AiShaderEvalParamRGBA(p_input));
}

