#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVIntMtd);

enum ReadAOVIntParams
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

struct ReadAOVIntData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVIntData *data = (ReadAOVIntData*) AiMalloc(sizeof(ReadAOVIntData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVIntData *data = (ReadAOVIntData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVIntData *data = (ReadAOVIntData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVIntData *data = (ReadAOVIntData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetInt(sg, data->aovName, sg->out.INT))
   {
      sg->out.INT = AiShaderEvalParamInt(p_default_value);
   }
}
