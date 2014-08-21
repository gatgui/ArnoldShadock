#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgIntMtd);

enum ReadMsgIntParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("msg_name", "");
   AiParameterInt("default_value", 0);
   
   AiMetaDataSetBool(mds, "msg_name", "linkable", false);
}

struct ReadMsgIntData
{
   const char *msgName;
};

node_initialize
{
   ReadMsgIntData *data = (ReadMsgIntData*) AiMalloc(sizeof(ReadMsgIntData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadMsgIntData *data = (ReadMsgIntData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, "msg_name");
}

node_finish
{
   ReadMsgIntData *data = (ReadMsgIntData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadMsgIntData *data = (ReadMsgIntData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgInt(data->msgName, &(sg->out.INT)))
   {
      sg->out.INT = AiShaderEvalParamInt(p_default_value);
   }
}
