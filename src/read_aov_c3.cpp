#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVC3Mtd);

enum ReadAOVC3Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterRGB("default_value", 0.0f, 0.0f, 0.0f);
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
   
   if (!AiAOVGetRGB(sg, data->aovName, sg->out.RGB))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
   }
}
