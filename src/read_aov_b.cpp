#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVBMtd);

enum ReadAOVBParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterBool("default_value", false);
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVBData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVBData *data = (ReadAOVBData*) AiMalloc(sizeof(ReadAOVBData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVBData *data = (ReadAOVBData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVBData *data = (ReadAOVBData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVBData *data = (ReadAOVBData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetBool(sg, data->aovName, sg->out.BOOL))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default_value);
   }
}
