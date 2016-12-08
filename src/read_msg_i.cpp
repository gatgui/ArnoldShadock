#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgIMtd);

enum ReadMsgIParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterInt("default_value", 0);
}

struct ReadMsgIData
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgIData());
   AddMemUsage<ReadMsgIData>();
}

node_update
{
   ReadMsgIData *data = (ReadMsgIData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgIData *data = (ReadMsgIData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgIData>();
}

shader_evaluate
{
   ReadMsgIData *data = (ReadMsgIData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgInt(data->msgName, &(sg->out.INT)))
   {
      sg->out.INT = AiShaderEvalParamInt(p_default_value);
   }
}
