#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVPnt2Mtd);

enum ReadAOVPnt2Params
{
   p_aov_name = 0
};

node_parameters
{
   AiParameterStr("aov_name", "");
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVPnt2Data
{
   const char *aovName;
};

node_initialize
{
   ReadAOVPnt2Data *data = (ReadAOVPnt2Data*) AiMalloc(sizeof(ReadAOVPnt2Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVPnt2Data *data = (ReadAOVPnt2Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVPnt2Data *data = (ReadAOVPnt2Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVPnt2Data *data = (ReadAOVPnt2Data*) AiNodeGetLocalData(node);
   
   AiAOVGetPnt2(sg, data->aovName, sg->out.PNT2);
}
