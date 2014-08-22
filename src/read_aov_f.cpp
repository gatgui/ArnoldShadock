#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVFMtd);

enum ReadAOVFParams
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

struct ReadAOVFData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVFData *data = (ReadAOVFData*) AiMalloc(sizeof(ReadAOVFData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVFData *data = (ReadAOVFData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVFData *data = (ReadAOVFData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVFData *data = (ReadAOVFData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetFlt(sg, data->aovName, sg->out.FLT))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default_value);
   }
}
