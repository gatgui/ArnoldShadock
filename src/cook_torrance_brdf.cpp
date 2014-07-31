#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(CookTorranceBrdfMtd);

enum CookTorranceBrdfParams
{
   p_roughness_x = 0,
   p_roughness_y,
   p_frame
};

node_parameters
{
   AiParameterFlt("roughness_x", 0.467f);
   AiParameterFlt("roughness_y", 0.467f);
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
   
   float rx = AiShaderEvalParamFlt(p_roughness_x);
   float ry = AiShaderEvalParamFlt(p_roughness_y);
   AtMatrix *frame = AiShaderEvalParamMtx(p_frame);
   
   AtVector U, V;
   
   U.x = (*frame)[0][0];
   U.y = (*frame)[0][1];
   U.z = (*frame)[0][2];
   
   V.x = (*frame)[1][0];
   V.y = (*frame)[1][1];
   V.z = (*frame)[1][2];
   
   brdf_data->evalSample = AiCookTorranceMISSample;
   brdf_data->evalBrdf = AiCookTorranceMISBRDF;
   brdf_data->evalPdf = AiCookTorranceMISPDF;
   brdf_data->data = AiCookTorranceMISCreateData(sg, &U, &V, rx, ry);
   
   sg->out.PTR = brdf_data;
}
