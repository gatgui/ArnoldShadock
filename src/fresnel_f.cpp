#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FresnelFMtd);

enum FresnelFParams
{
   p_view_vector = 0,
   p_normal,
   p_reflectance
};

node_parameters
{
   AiParameterVec(SSTR::view_vector, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::normal, 0.0f, 0.0f, 0.0f);
   AiParameterFlt(SSTR::reflectance, 1.0f);
}

struct FresnelFData
{
   bool evalV;
   bool evalN;
   float reflectance;
};

node_initialize
{
   AiNodeSetLocalData(node, new FresnelFData());
   AddMemUsage<FresnelFData>();
}

node_update
{
   FresnelFData *data = (FresnelFData*) AiNodeGetLocalData(node);
   
   data->evalV = AiNodeIsLinked(node, SSTR::view_vector);
   data->evalN = AiNodeIsLinked(node, SSTR::normal);
   data->reflectance = AiNodeGetFlt(node, SSTR::reflectance);
}

node_finish
{
   FresnelFData *data = (FresnelFData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<FresnelFData>();
}

shader_evaluate
{
   FresnelFData *data = (FresnelFData*) AiNodeGetLocalData(node);
   
   AtVector V = (data->evalV ? AiShaderEvalParamVec(p_view_vector) : sg->Rd);
   AtVector N = (data->evalN ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   sg->out.FLT = AiFresnelWeight(N, V, data->reflectance);
}
