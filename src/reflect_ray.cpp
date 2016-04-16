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
   AtRay *ray = 0;
   
   AiStateSetMsgPtr(SSTR::agsb_ray, 0);
   
   AiShaderEvalParamVec(p_ray);
   
   if (AiStateGetMsgPtr(SSTR::agsb_ray, (void**)&ray) && ray)
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
