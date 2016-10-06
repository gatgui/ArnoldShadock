#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVIMtd);

enum ReadAOVIParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterInt("default_value", 0);
   
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
   
   if (!AiAOVGetInt(sg, data->aovName, sg->out.INT))
   {
      sg->out.INT = AiShaderEvalParamInt(p_default_value);
   }
}
