#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgP2Mtd);

enum ReadMsgP2Params
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterPnt2("default_value", 0.0f, 0.0f);
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
   
   if (!AiStateGetMsgPnt2(data->msgName, &(sg->out.PNT2)))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default_value);
   }
}
