#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RefractVMtd);

enum RefractVParams
{
   p_direction = 0,
   p_normal,
   p_ior1,
   p_ior2
};

node_parameters
{
   AiParameterVec(SSTR::direction, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::normal, 0.0f, 0.0f, 0.0f);
   AiParameterFlt(SSTR::ior1, 1.0f);
   AiParameterFlt(SSTR::ior2, 1.0f);
   
   AiMetaDataSetBool(mds, SSTR::ior1, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::ior2, SSTR::linkable, false);
}

struct NodeData
{
   bool evalN;
   float ior1;
   float ior2;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->evalN = AiNodeIsLinked(node, SSTR::normal);
   data->ior1 = AiNodeGetFlt(node, SSTR::ior1);
   data->ior2 = AiNodeGetFlt(node, SSTR::ior2);
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
   
   AtVector D = AiShaderEvalParamVec(p_direction);
   AtVector N = (data->evalN ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   if (!AiRefract(&D, &N, &(sg->out.VEC), data->ior1, data->ior2))
   {
      // Total Internal Reflection
      sg->out.VEC = AI_V3_ZERO;
   }
}
