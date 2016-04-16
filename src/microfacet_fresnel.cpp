#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MicrofacetFresnelMtd);

enum MicrofacetNormalParams
{
   p_brdf = 0
};

node_parameters
{
   AiParameterRGB("brdf", 0.0f, 0.0f, 0.0f);
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
      sg->out.RGB = AiMicrofacetMISAverageFresnel(sg, brdf->data);
   }

   // Should I unset?
   //AiStateUnserMsgPtr(SSTR::agsb_brdf);
}
