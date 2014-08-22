#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgIMtd);

enum ReadMsgIParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("msg_name", "");
   AiParameterInt("default_value", 0);
   
   AiMetaDataSetBool(mds, "msg_name", "linkable", false);
}

struct ReadMsgIData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgIData *data = (ReadMsgIData*) AiMalloc(sizeof(ReadMsgIData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgIData *data = (ReadMsgIData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgIData *data = (ReadMsgIData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgIData *data = (ReadMsgIData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgInt(data->msgName, &(sg->out.INT)))
   {
      sg->out.INT = AiShaderEvalParamInt(p_default_value);
   }
}
