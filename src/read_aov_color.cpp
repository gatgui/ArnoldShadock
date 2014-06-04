#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVRGBMtd);

enum ReadAOVRGBParams
{
   p_aov_name = 0
};

node_parameters
{
   AiParameterStr("aov_name", "");
   
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
   
   AiAOVGetRGB(sg, data->aovName, sg->out.RGB);
}
