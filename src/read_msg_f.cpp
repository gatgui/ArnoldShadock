#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgFMtd);

enum ReadMsgFParams
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

struct ReadMsgFData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgFData *data = (ReadMsgFData*) AiMalloc(sizeof(ReadMsgFData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgFData *data = (ReadMsgFData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgFData *data = (ReadMsgFData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgFData *data = (ReadMsgFData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgFlt(data->msgName, &(sg->out.FLT)))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default_value);
   }
}
