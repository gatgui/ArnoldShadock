#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfEvalMtd);

enum BrdfEvalParams
{
   p_brdf_type = 0,
   p_mis_brdf,
   p_dir
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
   AiParameterVec("dir", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "brdf_type", "linkable", false);
}

node_initialize
{
}

node_update
{
   AiNodeSetLocalData(node, reinterpret_cast<void*>(AiNodeGetInt(node, "brdf_type")));
}

node_finish
{
}

shader_evaluate
{
   if (size_t(AiNodeGetLocalData(node)) == BT_MIS)
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
         AtVector dir = AiShaderEvalParamVec(p_dir);
         sg->out.RGB = brdf->evalBrdf(brdf->data, &dir);
      }
   }
   else
   {
      // Only LommelSeeliger
      AtVector dir = AiShaderEvalParamVec(p_dir);
      AtVector V = -sg->Rd;
      sg->out.RGB = AiLommelSeeligerBRDF(&dir, &V, &(sg->N));
   }
}
