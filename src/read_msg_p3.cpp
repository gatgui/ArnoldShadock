#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgP3Mtd);

enum ReadMsgP3Params
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

struct ReadMsgP3Data
{
   const char *msgName;
};

node_initialize
{
   ReadMsgP3Data *data = (ReadMsgP3Data*) AiMalloc(sizeof(ReadMsgP3Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgP3Data *data = (ReadMsgP3Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgP3Data *data = (ReadMsgP3Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgP3Data *data = (ReadMsgP3Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgPnt(data->msgName, &(sg->out.PNT)))
   {
      sg->out.PNT = AiShaderEvalParamPnt(p_default_value);
   }
}
