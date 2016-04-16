#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVP2Mtd);

enum ReadAOVP2Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterPnt2("default_value", 0.0f, 0.0f);
   
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
   
   if (!AiAOVGetPnt2(sg, data->aovName, sg->out.PNT2))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default_value);
   }
}
