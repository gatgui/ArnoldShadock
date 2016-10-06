#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgIMtd);

enum ReadMsgIParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterInt("default_value", 0);
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
   
   if (!AiStateGetMsgInt(data->msgName, &(sg->out.INT)))
   {
      sg->out.INT = AiShaderEvalParamInt(p_default_value);
   }
}
