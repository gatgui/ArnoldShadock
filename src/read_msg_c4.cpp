#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgC4Mtd);

enum ReadMsgC4Params
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

struct ReadMsgC4Data
{
   const char *msgName;
};

node_initialize
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiMalloc(sizeof(ReadMsgC4Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgRGBA(data->msgName, &(sg->out.RGBA)))
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_default_value);
   }
}
