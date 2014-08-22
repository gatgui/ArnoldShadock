#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgVMtd);

enum ReadMsgVParams
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

struct ReadMsgVData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgVData *data = (ReadMsgVData*) AiMalloc(sizeof(ReadMsgVData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgVData *data = (ReadMsgVData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgVData *data = (ReadMsgVData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgVData *data = (ReadMsgVData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgVec(data->msgName, &(sg->out.VEC)))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
   }
}
