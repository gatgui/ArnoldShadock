#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfEvalPdfMtd);

enum BrdfEvalPdfParams
{
   p_brdf = 0,
   p_dir
};

node_parameters
{
   AiParameterRGB("brdf", 0.0f, 0.0f, 0.0f);
   AiParameterVec("dir", 0.0f, 0.0f, 0.0f);
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
   BRDFData *brdf = 0;
      
   AiStateSetMsgPtr("agsb_brdf", 0);
   
   AiShaderEvalParamRGB(p_brdf);
   
   if (!AiStateGetMsgPtr("agsb_brdf", (void**)&brdf) || !brdf)
   {
      sg->out.FLT = 0.0f;
   }
   else
   {
      AtVector dir = AiShaderEvalParamVec(p_dir);
      sg->out.FLT = brdf->evalPdf(brdf->data, &dir);
   }
}
