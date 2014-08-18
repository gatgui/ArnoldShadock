#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RefractRayMtd);

enum RefractRayParams
{
   p_ray = 0,
   p_normal,
   p_n1,
   p_n2
};

node_parameters
{
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterVec("normal", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("n1", 1.0f);
   AiParameterFlt("n2", 1.0f);
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
      AiRefractRay(ray, (AiV3IsZero(N) ? &(sg->N) : &N),
                   AiShaderEvalParamFlt(p_n1), AiShaderEvalParamFlt(p_n2), sg);
      sg->out.VEC = ray->dir;
   }
   else
   {
      sg->out.VEC = AI_V3_ZERO;
   }
}
