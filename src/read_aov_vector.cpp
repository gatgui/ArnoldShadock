#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVVecMtd);

enum ReadAOVVecParams
{
   p_aov_name = 0
};

node_parameters
{
   AiParameterStr("aov_name", "");
   
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
   
   AiAOVGetVec(sg, data->aovName, sg->out.VEC);
}
