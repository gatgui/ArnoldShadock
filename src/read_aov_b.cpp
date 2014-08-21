#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVBoolMtd);

enum ReadAOVBoolParams
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

struct ReadAOVBoolData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVBoolData *data = (ReadAOVBoolData*) AiMalloc(sizeof(ReadAOVBoolData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVBoolData *data = (ReadAOVBoolData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVBoolData *data = (ReadAOVBoolData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVBoolData *data = (ReadAOVBoolData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetBool(sg, data->aovName, sg->out.BOOL))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default_value);
   }
}
