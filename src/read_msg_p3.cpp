#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgP3Mtd);

enum ReadMsgP3Params
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterPnt("default_value", 0.0f, 0.0f, 0.0f);
}

struct ReadMsgP3Data
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgP3Data());
   AddMemUsage<ReadMsgP3Data>();
}

node_update
{
   ReadMsgP3Data *data = (ReadMsgP3Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgP3Data *data = (ReadMsgP3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgP3Data>();
}

shader_evaluate
{
   ReadMsgP3Data *data = (ReadMsgP3Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgPnt(data->msgName, &(sg->out.PNT)))
   {
      sg->out.PNT = AiShaderEvalParamPnt(p_default_value);
   }
}
