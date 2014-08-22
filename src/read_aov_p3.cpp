#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVP3Mtd);

enum ReadAOVP3Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterPnt("default_value", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVP3Data
{
   const char *aovName;
};

node_initialize
{
   ReadAOVP3Data *data = (ReadAOVP3Data*) AiMalloc(sizeof(ReadAOVP3Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVP3Data *data = (ReadAOVP3Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVP3Data *data = (ReadAOVP3Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVP3Data *data = (ReadAOVP3Data*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetPnt(sg, data->aovName, sg->out.PNT))
   {
      sg->out.PNT = AiShaderEvalParamPnt(p_default_value);
   }
}
