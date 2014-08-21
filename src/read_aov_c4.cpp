#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVRGBAMtd);

enum ReadAOVRGBAParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterRGBA("default_value", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVRGBAData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVRGBAData *data = (ReadAOVRGBAData*) AiMalloc(sizeof(ReadAOVRGBAData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVRGBAData *data = (ReadAOVRGBAData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVRGBAData *data = (ReadAOVRGBAData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVRGBAData *data = (ReadAOVRGBAData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetRGBA(sg, data->aovName, sg->out.RGBA))
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_default_value);
   }
}
