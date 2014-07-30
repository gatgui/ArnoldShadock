#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(CookTorranceBrdfMtd);

enum CookTorranceBrdfParams
{
   p_roughness_x = 0,
   p_roughness_y
};

node_parameters
{
   AiParameterFlt("roughness_x", 0.467f);
   AiParameterFlt("roughness_y", 0.467f);
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
   BRDFData *brdf_data = (BRDFData*) AiShaderGlobalsQuickAlloc(sg, sizeof(BRDFData));
   
   float rx = AiShaderEvalParamFlt(p_roughness_x);
   float ry = AiShaderEvalParamFlt(p_roughness_y);
   AtVector u, v;
   
   if (!AiV3IsZero(sg->dPdu))
   {
      u = AiV3Normalize(sg->dPdu);
      v = AiV3Normalize(AiV3Cross(sg->Nf, u));
      u = AiV3Cross(v, sg->Nf);
   }
   else
   {
      AiBuildLocalFramePolar(&u, &v, &(sg->Nf));
   }
   
   brdf_data->evalSample = AiCookTorranceMISSample;
   brdf_data->evalBrdf = AiCookTorranceMISBRDF;
   brdf_data->evalPdf = AiCookTorranceMISPDF;
   brdf_data->data = AiCookTorranceMISCreateData(sg, &u, &v, rx, ry);
   
   sg->out.PTR = brdf_data;
}
