#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(IsValidRayMtd);

enum IsValidRayParams
{
   p_ray = 0
};

node_parameters
{
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
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
   AtRay *ray = 0;
   
   sg->out.BOOL = (AiStateGetMsgPtr("agsb_ray", (void**)&ray) && ray != 0);
}
