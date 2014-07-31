#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(AshikhminShirleyBrdfMtd);

enum AshikhminShirleyBrdfParams
{
   p_glossiness_x = 0,
   p_glossiness_y,
   p_frame
};

node_parameters
{
   AiParameterFlt("glossiness_x", 0.467f);
   AiParameterFlt("glossiness_y", 0.467f);
   AiParameterMtx("frame", AI_M4_IDENTITY);
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
   AtMatrix *frame = AiShaderEvalParamMtx(p_frame);
   
   AtVector U, V;
   
   U.x = (*frame)[0][0];
   U.y = (*frame)[0][1];
   U.z = (*frame)[0][2];
   
   V.x = (*frame)[1][0];
   V.y = (*frame)[1][1];
   V.z = (*frame)[1][2];
   
   brdf_data->evalSample = AiAshikhminShirleyMISSample;
   brdf_data->evalBrdf = AiAshikhminShirleyMISBRDF;
   brdf_data->evalPdf = AiAshikhminShirleyMISPDF;
   brdf_data->data = AiAshikhminShirleyMISCreateData(sg, &U, &V, gx, gy);
   
   sg->out.PTR = brdf_data;
}
