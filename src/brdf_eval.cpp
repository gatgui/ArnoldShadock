#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfEvalMtd);

enum BrdfEvalParams
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
   
   AiStateSetMsgPtr(SSTR::agsb_brdf, 0);
   
   AiShaderEvalParamRGB(p_brdf);
   
   if (!AiStateGetMsgPtr(SSTR::agsb_brdf, (void**)&brdf) || !brdf)
   {
      sg->out.RGB = AI_RGB_BLACK;
   }
   else
   {
      AtVector dir = AiShaderEvalParamVec(p_dir);
      sg->out.RGB = brdf->evalBrdf(brdf->data, &dir);
   }
}
