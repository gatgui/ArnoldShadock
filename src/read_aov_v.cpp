#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVVMtd);

enum ReadAOVVParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterVec("default_value", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVVData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVVData *data = (ReadAOVVData*) AiMalloc(sizeof(ReadAOVVData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVVData *data = (ReadAOVVData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVVData *data = (ReadAOVVData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVVData *data = (ReadAOVVData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetVec(sg, data->aovName, sg->out.VEC))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
   }
}
