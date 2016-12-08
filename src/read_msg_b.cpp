#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgBMtd);

enum ReadMsgBParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterBool("default_value", false);
}

struct ReadMsgBData
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgBData());
   AddMemUsage<ReadMsgBData>();
}

node_update
{
   ReadMsgBData *data = (ReadMsgBData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgBData *data = (ReadMsgBData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgBData>();
}

shader_evaluate
{
   ReadMsgBData *data = (ReadMsgBData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgBool(data->msgName, &(sg->out.BOOL)))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default_value);
   }
}
