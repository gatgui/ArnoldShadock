#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MakeRayMtd);

enum MakeRayParams
{
   p_type = 0,
   p_origin,
   p_direction,
   p_maxdist
};

node_parameters
{
   AiParameterEnum(SSTR::type, RT_Generic, RayTypeNames);
   AiParameterPnt(SSTR::origin, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::direction, 0.0f, 0.0f, 0.0f);
   AiParameterFlt("maxdist", AI_BIG);
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
   AtRay *ray = (AtRay*) AiShaderGlobalsQuickAlloc(sg, sizeof(AtRay));
   
   unsigned int type = RayTypeValues[AiShaderEvalParamInt(p_type)];
   AtPoint origin = AiShaderEvalParamPnt(p_origin);
   AtPoint dir = AiShaderEvalParamVec(p_direction);
   float maxdist = AiShaderEvalParamFlt(p_maxdist);
   
   AiMakeRay(ray, type, &origin, (AiV3IsZero(dir) ? NULL : &dir), maxdist, sg);
   AiStateSetMsgPtr(SSTR::agsb_ray, ray);
   
   sg->out.VEC = ray->dir;
}
