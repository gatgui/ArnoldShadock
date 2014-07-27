#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FresnelWeightFloatMtd);

enum FresnelWeightFloatParams
{
   p_view_vector = 0,
   p_normal,
   p_reflectance
};

node_parameters
{
   AiParameterVec("view_vector", 0.0f, 0.0f, 0.0f);
   AiParameterVec("normal", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("reflectance", 1.0f);
   
   AiMetaDataSetBool(mds, "reflectance", "linkable", false);
}

struct NodeData
{
   bool V_is_linked;
   bool N_is_linked;
   float reflectance;
};

node_initialize
{
   NodeData *data = (NodeData*) AiMalloc(sizeof(NodeData));
   AiNodeSetLocalData(node, data);
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->V_is_linked = AiNodeIsLinked(node, "view_vector");
   data->N_is_linked = AiNodeIsLinked(node, "normal");
   data->reflectance = AiNodeGetFlt(node, "reflectance");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   AtVector V = (data->V_is_linked ? AiShaderEvalParamVec(p_view_vector) : sg->Rd);
   AtVector N = (data->N_is_linked ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   sg->out.FLT = AiFresnelWeight(N, V, data->reflectance);
}
