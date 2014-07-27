#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVFltMtd);

enum ReadAOVFltParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterFlt("default_value", 0.0f);
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVFltData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVFltData *data = (ReadAOVFltData*) AiMalloc(sizeof(ReadAOVFltData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVFltData *data = (ReadAOVFltData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVFltData *data = (ReadAOVFltData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVFltData *data = (ReadAOVFltData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetFlt(sg, data->aovName, sg->out.FLT))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default_value);
   }
}
