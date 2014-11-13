#include "common.h"

#ifdef HAS_OBJECT_MATTE

AI_SHADER_NODE_EXPORT_METHODS(IsMatteMtd);

node_parameters
{
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
   sg->out.BOOL = AiShaderGlobalsIsObjectMatte(sg);
}

#endif