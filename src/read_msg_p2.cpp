#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgP2Mtd);

enum ReadMsgP2Params
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("msg_name", "");
   AiParameterPnt2("default_value", 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "msg_name", "linkable", false);
}

struct ReadMsgP2Data
{
   const char *msgName;
};

node_initialize
{
   ReadMsgP2Data *data = (ReadMsgP2Data*) AiMalloc(sizeof(ReadMsgP2Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgP2Data *data = (ReadMsgP2Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgP2Data *data = (ReadMsgP2Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgP2Data *data = (ReadMsgP2Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgPnt2(data->msgName, &(sg->out.PNT2)))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default_value);
   }
}
