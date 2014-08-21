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
   AtRay *ray = 0;
   
   AiStateSetMsgPtr("agsb_ray", 0);
   
   AiShaderEvalParamVec(p_ray);
   
   if (AiStateGetMsgPtr("agsb_ray", (void**)&ray) && ray)
   {
      AtVector N = AiShaderEvalParamVec(p_normal);
      if (!AiRefractRay(ray, (AiV3IsZero(N) ? &(sg->N) : &N),
                        AiShaderEvalParamFlt(p_n1),
                        AiShaderEvalParamFlt(p_n2), sg))
      {
         sg->out.VEC = AI_V3_ZERO;
         AiStateSetMsgPtr("agsb_ray", 0);
      }
      else
      {
         sg->out.VEC = ray->dir;
      }
   }
   else
   {
      sg->out.VEC = AI_V3_ZERO;
   }
}
