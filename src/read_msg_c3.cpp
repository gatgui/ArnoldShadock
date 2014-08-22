#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgC3Mtd);

enum ReadMsgC3Params
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

struct ReadMsgC3Data
{
   const char *msgName;
};

node_initialize
{
   ReadMsgC3Data *data = (ReadMsgC3Data*) AiMalloc(sizeof(ReadMsgC3Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgC3Data *data = (ReadMsgC3Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgC3Data *data = (ReadMsgC3Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgC3Data *data = (ReadMsgC3Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgRGB(data->msgName, &(sg->out.RGB)))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
   }
}
