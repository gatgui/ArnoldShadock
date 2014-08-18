#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MakeRayMtd);

enum MakeRayParams
{
   p_type = 0,
   p_origin,
   p_direction,
   p_maxdist
};

enum RayType
{
   RT_generic = 0,
   RT_camera,
   RT_shadow,
   RT_reflected,
   RT_refracted,
   RT_diffuse,
   RT_glossy
};

static const char* RayTypeNames[] = {"genenric", "camera", "shadow", "reflected", "refracted", "diffuse", "glossy", NULL};

static unsigned int RayTypeValues[] = {AI_RAY_GENERIC, AI_RAY_CAMERA, AI_RAY_SHADOW, AI_RAY_REFLECTED,
                                       AI_RAY_REFRACTED, AI_RAY_DIFFUSE, AI_RAY_GLOSSY};

node_parameters
{
   AiParameterEnum("type", RT_generic, RayTypeNames);
   AiParameterPnt("origin", 0.0f, 0.0f, 0.0f);
   AiParameterVec("direction", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("maxdist", 1.0f);
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
   AiStateSetMsgPtr("ray", ray);
   
   sg->out.VEC = ray->dir;
}
