#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgC3Mtd);

enum ReadMsgC3Params
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterRGB("default_value", 0.0f, 0.0f, 0.0f);
}

struct ReadMsgC3Data
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgC3Data());
   AddMemUsage<ReadMsgC3Data>();
}

node_update
{
   ReadMsgC3Data *data = (ReadMsgC3Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgC3Data *data = (ReadMsgC3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgC3Data>();
}

shader_evaluate
{
   ReadMsgC3Data *data = (ReadMsgC3Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgRGB(data->msgName, &(sg->out.RGB)))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
   }
}
