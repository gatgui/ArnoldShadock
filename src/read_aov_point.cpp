#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVPntMtd);

enum ReadAOVPntParams
{
   p_aov_name = 0
};

node_parameters
{
   AiParameterStr("aov_name", "");
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVPntData
{
   const char *aovName;
};

node_initialize
{
   ReadAOVPntData *data = (ReadAOVPntData*) AiMalloc(sizeof(ReadAOVPntData));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVPntData *data = (ReadAOVPntData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVPntData *data = (ReadAOVPntData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVPntData *data = (ReadAOVPntData*) AiNodeGetLocalData(node);
   
   AiAOVGetPnt(sg, data->aovName, sg->out.PNT);
}
