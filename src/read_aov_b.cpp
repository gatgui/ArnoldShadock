#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVBMtd);

enum ReadAOVBParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterBool("default_value", false);
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
   
   if (!AiAOVGetBool(sg, data->aovName, sg->out.BOOL))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default_value);
   }
}
