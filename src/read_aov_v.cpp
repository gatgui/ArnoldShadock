#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVVMtd);

enum ReadAOVVParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterVec("default_value", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, SSTR::aov_name, SSTR::linkable, false);
}

struct NodeData
{
   AtString aovName;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
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
   
   if (!AiAOVGetVec(sg, data->aovName, sg->out.VEC))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
   }
}
