#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVVecMtd);

enum ReadAOVVecParams
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

struct ReadAOVVecData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVVecData *data = (ReadAOVVecData*) AiMalloc(sizeof(ReadAOVVecData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVVecData *data = (ReadAOVVecData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVVecData *data = (ReadAOVVecData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVVecData *data = (ReadAOVVecData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetVec(sg, data->aovName, sg->out.VEC))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
   }
}
