#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(AshikhminShirleyBrdfMtd);

enum AshikhminShirleyBrdfParams
{
   p_glossiness_x = 0,
   p_glossiness_y
};

node_parameters
{
   AiParameterFlt("glossiness_x", 0.467f);
   AiParameterFlt("glossiness_y", 0.467f);
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
   
   float gx = AiShaderEvalParamFlt(p_glossiness_x);
   float gy = AiShaderEvalParamFlt(p_glossiness_y);
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
   
   brdf_data->evalSample = AiAshikhminShirleyMISSample;
   brdf_data->evalBrdf = AiAshikhminShirleyMISBRDF;
   brdf_data->evalPdf = AiAshikhminShirleyMISPDF;
   brdf_data->data = AiAshikhminShirleyMISCreateData(sg, &u, &v, gx, gy);
   
   sg->out.PTR = brdf_data;
}
