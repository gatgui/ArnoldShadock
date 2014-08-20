#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfIntegrateMtd);

enum BrdfIntegrateParams
{
   p_brdf = 0,
   p_ray_type,
};

node_parameters
{
   AiParameterRGB("brdf", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("ray_type", RT_Diffuse, RayTypeNames);
   
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
   
   AiStateSetMsgPtr("agsb_brdf", 0);
   
   AiShaderEvalParamRGB(p_brdf);
   
   if (!AiStateGetMsgPtr("agsb_brdf", (void**)&brdf) || !brdf)
   {
      sg->out.RGB = AI_RGB_BLACK;
   }
   else
   {
      sg->out.RGB = AiBRDFIntegrate(sg, brdf->data, brdf->evalSample, brdf->evalBrdf, brdf->evalPdf, RayTypeValues[data->ray_type]);
   }
}
