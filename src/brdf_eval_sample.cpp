#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfEvalSampleMtd);

enum BrdfEvalSampleParams
{
   p_brdf = 0,
   p_sample
};

node_parameters
{
   AiParameterRGB("brdf", 0.0f, 0.0f, 0.0f);
   AiParameterPnt2("sample", 0.0f, 0.0f);
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
      sg->out.VEC = AI_V3_ZERO;
   }
   else
   {
      AtPoint2 sample = AiShaderEvalParamPnt2(p_sample);
      sg->out.VEC = brdf->evalSample(brdf->data, sample.x, sample.y);
   }
}
