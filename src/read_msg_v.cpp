#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgVMtd);

enum ReadMsgVParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterVec("default_value", 0.0f, 0.0f, 0.0f);
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
   
   if (!AiStateGetMsgVec(data->msgName, &(sg->out.VEC)))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
   }
}
