#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVP2Mtd);

enum ReadAOVP2Params
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

struct ReadAOVP2Data
{
   const char *aovName;
};

node_initialize
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiMalloc(sizeof(ReadAOVP2Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetPnt2(sg, data->aovName, sg->out.PNT2))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default_value);
   }
}
