#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfOrenNayarMtd);

enum BrdfOrenNayarParams
{
   p_roughness = 0
};

node_parameters
{
   AiParameterFlt("roughness", 0.0f);
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
   BRDFData *data = (BRDFData*) AiShaderGlobalsQuickAlloc(sg, sizeof(BRDFData));
   
   data->evalSample = AiOrenNayarMISSample;
   data->evalBrdf = AiOrenNayarMISBRDF;
   data->evalPdf = AiOrenNayarMISPDF;
   data->data = AiOrenNayarMISCreateData(sg, AiShaderEvalParamFlt(p_roughness));
   
   AiStateSetMsgPtr(SSTR::agsb_brdf, data);
   
   sg->out.RGB = AI_RGB_BLACK;
}
