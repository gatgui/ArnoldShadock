#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SssSingleMtd);

enum SssSingleParams
{
   p_Rd = 0,
   p_mfp,
   p_g,
   p_eta
};

node_parameters
{
   AiParameterRGB("Rd", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("mfp", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("g", 0.0f);
   AiParameterFlt("eta", 1.0f);
   
   AiMetaDataSetFlt(mds, "g", "min", -1.0);
   AiMetaDataSetFlt(mds, "g", "max", -1.0);
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
   AtRGB Rd = AiShaderEvalParamRGB(p_Rd);
   AtRGB mfp = AiShaderEvalParamRGB(p_mfp);
   float g = AiShaderEvalParamFlt(p_g);
   float eta = AiShaderEvalParamFlt(p_eta);
   
   sg->out.RGB = AiSSSTraceSingleScatter(sg, Rd, mfp, g, eta);
}
