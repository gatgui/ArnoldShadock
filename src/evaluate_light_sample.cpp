#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(EvaluateLightSampleMtd);

enum EvaluateLightSampleParams
{
   p_brdf = 0
};

node_parameters
{
   AiParameterPtr("brdf", 0);
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
      sg->out.RGB = AiEvaluateLightSample(sg, brdf->data, brdf->evalSample, brdf->evalBrdf, brdf->evalPdf);
   }
}
