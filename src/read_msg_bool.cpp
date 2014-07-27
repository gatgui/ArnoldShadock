#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgBoolMtd);

enum ReadMsgBoolParams
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

struct ReadMsgBoolData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgBoolData *data = (ReadMsgBoolData*) AiMalloc(sizeof(ReadMsgBoolData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgBoolData *data = (ReadMsgBoolData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgBoolData *data = (ReadMsgBoolData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgBoolData *data = (ReadMsgBoolData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgBool(data->msgName, &(sg->out.BOOL)))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default_value);
   }
}
