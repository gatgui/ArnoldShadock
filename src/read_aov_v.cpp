#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVVMtd);

enum ReadAOVVParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterVec("default_value", 0.0f, 0.0f, 0.0f);
}

struct ReadAOVVData
{
   AtString aovName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadAOVVData());
   AddMemUsage<ReadAOVVData>();
}

node_update
{
   ReadAOVVData *data = (ReadAOVVData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVVData *data = (ReadAOVVData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadAOVVData>();
}

shader_evaluate
{
   ReadAOVVData *data = (ReadAOVVData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetVec(sg, data->aovName, sg->out.VEC))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
   }
}
