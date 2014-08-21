#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgPntMtd);

enum ReadMsgPntParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("msg_name", "");
   AiParameterPnt("default_value", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "msg_name", "linkable", false);
}

struct ReadMsgPntData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgPntData *data = (ReadMsgPntData*) AiMalloc(sizeof(ReadMsgPntData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgPntData *data = (ReadMsgPntData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgPntData *data = (ReadMsgPntData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgPntData *data = (ReadMsgPntData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgPnt(data->msgName, &(sg->out.PNT)))
   {
      sg->out.PNT = AiShaderEvalParamPnt(p_default_value);
   }
}
