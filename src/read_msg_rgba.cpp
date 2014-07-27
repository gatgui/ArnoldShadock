#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgRGBAMtd);

enum ReadMsgRGBAParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("msg_name", "");
   AiParameterRGBA("default_value", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiMetaDataSetBool(mds, "msg_name", "linkable", false);
}

struct ReadMsgRGBAData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgRGBAData *data = (ReadMsgRGBAData*) AiMalloc(sizeof(ReadMsgRGBAData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgRGBAData *data = (ReadMsgRGBAData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgRGBAData *data = (ReadMsgRGBAData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgRGBAData *data = (ReadMsgRGBAData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgRGBA(data->msgName, &(sg->out.RGBA)))
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_default_value);
   }
}
