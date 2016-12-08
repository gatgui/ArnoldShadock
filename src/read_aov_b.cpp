#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVBMtd);

enum ReadAOVBParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterBool("default_value", false);
}

struct ReadAOVBData
{
   AtString aovName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadAOVBData());
   AddMemUsage<ReadAOVBData>();
}

node_update
{
   ReadAOVBData *data = (ReadAOVBData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVBData *data = (ReadAOVBData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadAOVBData>();
}

shader_evaluate
{
   ReadAOVBData *data = (ReadAOVBData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetBool(sg, data->aovName, sg->out.BOOL))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default_value);
   }
}
