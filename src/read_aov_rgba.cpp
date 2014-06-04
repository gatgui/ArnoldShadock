#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVRGBAMtd);

enum ReadAOVRGBAParams
{
   p_aov_name = 0
};

node_parameters
{
   AiParameterStr("aov_name", "");
   
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
   
   AiAOVGetRGBA(sg, data->aovName, sg->out.RGBA);
}
