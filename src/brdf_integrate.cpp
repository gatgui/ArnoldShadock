#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfIntegrateMtd);

enum BrdfIntegrateParams
{
   p_brdf_type = 0,
   p_mis_brdf,
   p_mis_ray_type
};

enum BrdfType
{
   BT_MIS = 0,
   BT_LommelSeeliger
};

static const char* BrdfTypeNames[] = {"mis", "lommel_seeliger", NULL};

node_parameters
{
   AiParameterEnum("brdf_type", BT_MIS, BrdfTypeNames);
   AiParameterRGB("mis_brdf", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("mis_ray_type", RT_Diffuse, RayTypeNames);
   
   AiMetaDataSetBool(mds, "brdf_type", "linkable", false);
   AiMetaDataSetBool(mds, "mis_ray_type", "linkable", false);
}

struct BrdfIntegrateData
{
   BrdfType brdf_type;
   int mis_ray_type;
   
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(BrdfIntegrateData)));
}

node_update
{
   BrdfIntegrateData *data = (BrdfIntegrateData*) AiNodeGetLocalData(node);
   
   data->brdf_type = (BrdfType) AiNodeGetInt(node, "brdf_type");
   data->mis_ray_type = AiNodeGetInt(node, "mis_ray_type");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   BrdfIntegrateData *data = (BrdfIntegrateData*) AiNodeGetLocalData(node);
   
   if (data->brdf_type == BT_MIS)
   {
      BRDFData *brdf = 0;
      
      AiStateSetMsgPtr("agsb_brdf", 0);
      
      AiShaderEvalParamRGB(p_mis_brdf);
      
      if (!AiStateGetMsgPtr("agsb_brdf", (void**)&brdf) || !brdf)
      {
         sg->out.RGB = AI_RGB_BLACK;
      }
      else
      {
         sg->out.RGB = AiBRDFIntegrate(sg, brdf->data, brdf->evalSample, brdf->evalBrdf, brdf->evalPdf, RayTypeValues[data->mis_ray_type]);
      }
   }
   else
   {
      // Only LommelSeeliger
      sg->out.RGB = AiLommelSeeligerIntegrate(&(sg->N), sg);
   }
}
