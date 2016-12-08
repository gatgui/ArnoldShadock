#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgFMtd);

enum ReadMsgFParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterFlt("default_value", 0.0f);
}

struct ReadMsgFData
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgFData());
   AddMemUsage<ReadMsgFData>();
}

node_update
{
   ReadMsgFData *data = (ReadMsgFData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgFData *data = (ReadMsgFData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgFData>();
}

shader_evaluate
{
   ReadMsgFData *data = (ReadMsgFData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgFlt(data->msgName, &(sg->out.FLT)))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default_value);
   }
}
