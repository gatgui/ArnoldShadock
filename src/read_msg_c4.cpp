#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgC4Mtd);

enum ReadMsgC4Params
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterRGBA("default_value", 0.0f, 0.0f, 0.0f, 1.0f);
}

struct NodeData
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
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
   
   if (!AiStateGetMsgRGBA(data->msgName, &(sg->out.RGBA)))
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_default_value);
   }
}
