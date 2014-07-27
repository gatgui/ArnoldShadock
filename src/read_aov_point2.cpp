#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVPnt2Mtd);

enum ReadAOVPnt2Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterPnt2("default_value", 0.0f, 0.0f);
   
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
   
   if (!AiAOVGetPnt2(sg, data->aovName, sg->out.PNT2))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default_value);
   }
}
