#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SetRayDirMtd);

enum SetRayDirParams
{
   p_ray = 0,
   p_dir
};

node_parameters
{
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterVec("dir", 0.0f, 0.0f, 0.0f);
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
   
   AiStateSetMsgPtr("agsb_ray", 0);
   
   AiShaderEvalParamVec(p_ray);
   
   if (AiStateGetMsgPtr("agsb_ray", (void**)&ray) && ray)
   {
      ray->dir = AiShaderEvalParamVec(p_dir);
      sg->out.VEC = ray->dir;
   }
   else
   {
      sg->out.VEC = AI_V3_ZERO;
   }
}
