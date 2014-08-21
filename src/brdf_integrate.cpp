#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfIntegrateMtd);

enum BrdfIntegrateParams
{
   p_BRDF = 0,
   p_MIS_brdf,
   p_MIS_ray_type
};

enum BRDF
{
   BRDF_MIS = 0,
   BRDF_LommelSeeliger
};

static const char* BRDFNames[] = {"MIS", "lommel_seeliger", NULL};

node_parameters
{
   AiParameterEnum("BRDF", BRDF_MIS, BRDFNames);
   AiParameterRGB("MIS_brdf", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("MIS_ray_type", RT_Diffuse, RayTypeNames);
   
   AiMetaDataSetBool(mds, "BRDF", "linkable", false);
   AiMetaDataSetBool(mds, "MIS_ray_type", "linkable", false);
}

struct BrdfIntegrateData
{
   BRDF brdf;
   int MIS_ray_type;
   
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(BrdfIntegrateData)));
}

node_update
{
   BrdfIntegrateData *data = (BrdfIntegrateData*) AiNodeGetLocalData(node);
   
   data->brdf = (BRDF) AiNodeGetBool(node, "BRDF");
   data->MIS_ray_type = AiNodeGetInt(node, "MIS_ray_type");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   BrdfIntegrateData *data = (BrdfIntegrateData*) AiNodeGetLocalData(node);
   
   if (data->brdf == BRDF_MIS)
   {
      BRDFData *brdf = 0;
      
      AiStateSetMsgPtr("agsb_brdf", 0);
      
      AiShaderEvalParamRGB(p_MIS_brdf);
      
      if (!AiStateGetMsgPtr("agsb_brdf", (void**)&brdf) || !brdf)
      {
         sg->out.RGB = AI_RGB_BLACK;
      }
      else
      {
         sg->out.RGB = AiBRDFIntegrate(sg, brdf->data, brdf->evalSample, brdf->evalBrdf, brdf->evalPdf, RayTypeValues[data->MIS_ray_type]);
      }
   }
   else
   {
      // Only LommelSeeliger
      sg->out.RGB = AiLommelSeeligerIntegrate(&(sg->N), sg);
   }
}
