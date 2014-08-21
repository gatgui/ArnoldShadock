#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVRGBMtd);

enum ReadAOVRGBParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterRGB("default_value", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVRGBData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVRGBData *data = (ReadAOVRGBData*) AiMalloc(sizeof(ReadAOVRGBData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVRGBData *data = (ReadAOVRGBData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVRGBData *data = (ReadAOVRGBData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVRGBData *data = (ReadAOVRGBData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetRGB(sg, data->aovName, sg->out.RGB))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
   }
}
