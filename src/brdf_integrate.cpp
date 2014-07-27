#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfIntegrateMtd);

enum BrdfIntegrateParams
{
   p_brdf = 0,
   p_ray_type
};

enum RayType
{
   RT_camera = 0,
   RT_shadow,
   RT_reflected,
   RT_refracted,
   RT_diffuse,
   RT_glossy
};

static const char* RayTypeNames[] = 
{
   "camera",
   "shadow",
   "reflected",
   "refracted",
   "diffuse",
   "glossy",
   NULL
};

static AtUInt16 RayTypeValues[] =
{
   AI_RAY_CAMERA,
   AI_RAY_SHADOW,
   AI_RAY_REFLECTED,
   AI_RAY_REFRACTED,
   AI_RAY_DIFFUSE,
   AI_RAY_GLOSSY
};

node_parameters
{
   AiParameterPtr("brdf", 0);
   AiParameterEnum("ray_type", RT_diffuse, RayTypeNames)
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
   BRDFData *brdf = (BRDFData*) AiShaderEvalParamPtr(p_brdf);
   
   if (!brdf)
   {
      sg->out.RGB = AI_RGB_BLACK;
   }
   else
   {
      int ray_type = AiShaderEvalParamInt(p_ray_type);
      sg->out.RGB = AiBRDFIntegrate(sg, brdf->data, brdf->evalSample, brdf->evalBrdf, brdf->evalPdf, RayTypeValues[ray_type]);
   }
}
