#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVIMtd);

enum ReadAOVIParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterInt("default_value", 0);
}

struct ReadAOVIData
{
   AtString aovName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadAOVIData());
   AddMemUsage<ReadAOVIData>();
}

node_update
{
   ReadAOVIData *data = (ReadAOVIData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVIData *data = (ReadAOVIData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadAOVIData>();
}

shader_evaluate
{
   ReadAOVIData *data = (ReadAOVIData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetInt(sg, data->aovName, sg->out.INT))
   {
      sg->out.INT = AiShaderEvalParamInt(p_default_value);
   }
}
