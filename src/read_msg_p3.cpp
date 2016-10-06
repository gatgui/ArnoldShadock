#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgP3Mtd);

enum ReadMsgP3Params
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterPnt("default_value", 0.0f, 0.0f, 0.0f);
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
   
   if (!AiStateGetMsgPnt(data->msgName, &(sg->out.PNT)))
   {
      sg->out.PNT = AiShaderEvalParamPnt(p_default_value);
   }
}
