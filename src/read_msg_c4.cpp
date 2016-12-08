#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadMsgC4Mtd);

enum ReadMsgC4Params
{
   p_msg_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::msg_name, "");
   AiParameterRGBA("default_value", 0.0f, 0.0f, 0.0f, 1.0f);
}

struct ReadMsgC4Data
{
   AtString msgName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadMsgC4Data());
   AddMemUsage<ReadMsgC4Data>();
}

node_update
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiNodeGetLocalData(node);
   
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
}

node_finish
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadMsgC4Data>();
}

shader_evaluate
{
   ReadMsgC4Data *data = (ReadMsgC4Data*) AiNodeGetLocalData(node);
   
   if (!AiStateGetMsgRGBA(data->msgName, &(sg->out.RGBA)))
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_default_value);
   }
}
