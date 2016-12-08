#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgP2Mtd);

enum ReadMsgP2Params
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterPnt2("default_value", 0.0f, 0.0f);
}

struct ReadMsgP2Data
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgP2Data());
   AddMemUsage<ReadMsgP2Data>();
}

node_update
{
   ReadMsgP2Data *data = (ReadMsgP2Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgP2Data *data = (ReadMsgP2Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgP2Data>();
}

shader_evaluate
{
   ReadMsgP2Data *data = (ReadMsgP2Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgPnt2(data->msgName, &(sg->out.PNT2)))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default_value);
   }
}
