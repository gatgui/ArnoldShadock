#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgVecMtd);

enum ReadMsgVecParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("msg_name", "");
   AiParameterVec("default_value", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "msg_name", "linkable", false);
}

struct ReadMsgVecData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgVecData *data = (ReadMsgVecData*) AiMalloc(sizeof(ReadMsgVecData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgVecData *data = (ReadMsgVecData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgVecData *data = (ReadMsgVecData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgVecData *data = (ReadMsgVecData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgVec(data->msgName, &(sg->out.VEC)))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
   }
}
