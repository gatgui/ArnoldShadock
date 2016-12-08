#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgVMtd);

enum ReadMsgVParams
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterVec("default_value", 0.0f, 0.0f, 0.0f);
}

struct ReadMsgVData
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgVData());
   AddMemUsage<ReadMsgVData>();
}

node_update
{
   ReadMsgVData *data = (ReadMsgVData*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgVData *data = (ReadMsgVData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgVData>();
}

shader_evaluate
{
   ReadMsgVData *data = (ReadMsgVData*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgVec(data->msgName, &(sg->out.VEC)))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
   }
}
