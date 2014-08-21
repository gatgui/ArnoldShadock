#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgFltMtd);

enum ReadMsgFltParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("msg_name", "");
   AiParameterFlt("default_value", 0.0f);
   
   AiMetaDataSetBool(mds, "msg_name", "linkable", false);
}

struct ReadMsgFltData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgFltData *data = (ReadMsgFltData*) AiMalloc(sizeof(ReadMsgFltData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgFltData *data = (ReadMsgFltData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgFltData *data = (ReadMsgFltData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgFltData *data = (ReadMsgFltData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgFlt(data->msgName, &(sg->out.FLT)))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default_value);
   }
}
