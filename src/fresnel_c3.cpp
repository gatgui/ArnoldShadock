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

struct NodeData
{
   bool evalV;
   bool evalN;
   AtColor reflectance;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->evalV = AiNodeIsLinked(node, SSTR::view_vector);
   data->evalN = AiNodeIsLinked(node, SSTR::normal);
   data->reflectance = AiNodeGetRGB(node, SSTR::reflectance);
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   AtVector V = (data->evalV ? AiShaderEvalParamVec(p_view_vector) : sg->Rd);
   AtVector N = (data->evalN ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   AiFresnelWeightRGB(&N, &V, &(data->reflectance), &(sg->out.RGB));
}
