#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FresnelC3Mtd);

enum FresnelC3Params
{
   p_view_vector = 0,
   p_normal,
   p_reflectance
};

node_parameters
{
   AiParameterVec(SSTR::view_vector, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::normal, 0.0f, 0.0f, 0.0f);
   AiParameterRGB(SSTR::reflectance, 1.0f, 1.0f, 1.0f);
}

struct FresnelC3Data
{
   bool evalV;
   bool evalN;
   AtColor reflectance;
};

node_initialize
{
   AiNodeSetLocalData(node, new FresnelC3Data());
   AddMemUsage<FresnelC3Data>();
}

node_update
{
   FresnelC3Data *data = (FresnelC3Data*) AiNodeGetLocalData(node);
   
   data->evalV = AiNodeIsLinked(node, SSTR::view_vector);
   data->evalN = AiNodeIsLinked(node, SSTR::normal);
   data->reflectance = AiNodeGetRGB(node, SSTR::reflectance);
}

node_finish
{
   FresnelC3Data *data = (FresnelC3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<FresnelC3Data>();
}

shader_evaluate
{
   FresnelC3Data *data = (FresnelC3Data*) AiNodeGetLocalData(node);
   
   AtVector V = (data->evalV ? AiShaderEvalParamVec(p_view_vector) : sg->Rd);
   AtVector N = (data->evalN ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   AiFresnelWeightRGB(&N, &V, &(data->reflectance), &(sg->out.RGB));
}
