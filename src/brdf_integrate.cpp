#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfIntegrateMtd);

enum BrdfIntegrateParams
{
   p_brdf = 0,
   p_ray_type,
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
   AiParameterRGB("brdf", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("ray_type", RT_diffuse, RayTypeNames);
   
   AiMetaDataSetBool(mds, "ray_type", "linkable", false);
}

struct BrdfIntegrateData
{
   int ray_type;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(BrdfIntegrateData)));
}

node_update
{
   BrdfIntegrateData *data = (BrdfIntegrateData*) AiNodeGetLocalData(node);
   
   data->ray_type = AiNodeGetInt(node, "ray_type");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   BrdfIntegrateData *data = (BrdfIntegrateData*) AiNodeGetLocalData(node);
   
   BRDFData *brdf = 0;
   
   AiStateSetMsgPtr("brdf", 0);
   
   AiShaderEvalParamRGB(p_brdf);
   
   if (!AiStateGetMsgPtr("brdf", (void**)&brdf) || !brdf)
   {
      sg->out.RGB = AI_RGB_BLACK;
   }
   else
   {
      sg->out.RGB = AiBRDFIntegrate(sg, brdf->data, brdf->evalSample, brdf->evalBrdf, brdf->evalPdf, RayTypeValues[data->ray_type]);
   }
}
