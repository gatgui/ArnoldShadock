#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVIMtd);

enum ReadAOVIParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterInt("default_value", 0);
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVIData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVIData *data = (ReadAOVIData*) AiMalloc(sizeof(ReadAOVIData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVIData *data = (ReadAOVIData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVIData *data = (ReadAOVIData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVIData *data = (ReadAOVIData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetInt(sg, data->aovName, sg->out.INT))
   {
      sg->out.INT = AiShaderEvalParamInt(p_default_value);
   }
}
