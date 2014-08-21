#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgPnt2Mtd);

enum ReadMsgPnt2Params
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

struct ReadMsgPnt2Data
{
   const char *msgName;
};

node_initialize
{
   ReadMsgPnt2Data *data = (ReadMsgPnt2Data*) AiMalloc(sizeof(ReadMsgPnt2Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgPnt2Data *data = (ReadMsgPnt2Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgPnt2Data *data = (ReadMsgPnt2Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgPnt2Data *data = (ReadMsgPnt2Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgPnt2(data->msgName, &(sg->out.PNT2)))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default_value);
   }
}
