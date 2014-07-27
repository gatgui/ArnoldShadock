#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(OrenNayarBrdfMtd);

enum OrenNayarBrdfParams
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
   
   sg->out.PTR = data;
}
