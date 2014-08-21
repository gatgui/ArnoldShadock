#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgRGBMtd);

enum ReadMsgRGBParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("msg_name", "");
   AiParameterRGB("default_value", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "msg_name", "linkable", false);
}

struct ReadMsgRGBData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgRGBData *data = (ReadMsgRGBData*) AiMalloc(sizeof(ReadMsgRGBData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgRGBData *data = (ReadMsgRGBData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgRGBData *data = (ReadMsgRGBData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgRGBData *data = (ReadMsgRGBData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgRGB(data->msgName, &(sg->out.RGB)))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
   }
}
