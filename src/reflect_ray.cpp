#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReflectRayMtd);

enum ReflectRayParams
{
   p_ray = 0,
   p_normal
};

node_parameters
{
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterVec("normal", 0.0f, 0.0f, 0.0f);
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
   AiShaderEvalParamVec(p_ray);
   
   AtRay *ray = 0;
   
   if (AiStateGetMsgPtr("ray", (void**)&ray))
   {
      AtVector N = AiShaderEvalParamVec(p_normal);
      AiReflectRay(ray, (AiV3IsZero(N) ? &(sg->N) : &N), sg);
      sg->out.VEC = ray->dir;
   }
   else
   {
      sg->out.VEC = AI_V3_ZERO;
   }
}
