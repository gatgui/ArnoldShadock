#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfStretchedPhongMtd);

enum BrdfStretchedPhongParams
{
   p_exponent = 0
};

node_parameters
{
   AiParameterFlt("exponent", 0.5f);
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
   
   data->evalSample = AiStretchedPhongMISSample;
   data->evalBrdf = AiStretchedPhongMISBRDF;
   data->evalPdf = AiStretchedPhongMISPDF;
   data->data = AiStretchedPhongMISCreateData(sg, AiShaderEvalParamFlt(p_exponent));
   
   AiStateSetMsgPtr(SSTR::agsb_brdf, data);
   
   sg->out.RGB = AI_RGB_BLACK;
}
