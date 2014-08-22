#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgBMtd);

enum ReadMsgBParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("msg_name", "");
   AiParameterBool("default_value", false);
   
   AiMetaDataSetBool(mds, "msg_name", "linkable", false);
}

struct ReadMsgBData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgBData *data = (ReadMsgBData*) AiMalloc(sizeof(ReadMsgBData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgBData *data = (ReadMsgBData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgBData *data = (ReadMsgBData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgBData *data = (ReadMsgBData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgBool(data->msgName, &(sg->out.BOOL)))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default_value);
   }
}
