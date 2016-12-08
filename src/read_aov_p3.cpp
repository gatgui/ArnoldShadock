#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVP3Mtd);

enum ReadAOVP3Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterPnt("default_value", 0.0f, 0.0f, 0.0f);
}

struct ReadAOVP3Data
{
   AtString aovName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadAOVP3Data());
   AddMemUsage<ReadAOVP3Data>();
}

node_update
{
   ReadAOVP3Data *data = (ReadAOVP3Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVP3Data *data = (ReadAOVP3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadAOVP3Data>();
}

shader_evaluate
{
   ReadAOVP3Data *data = (ReadAOVP3Data*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetPnt(sg, data->aovName, sg->out.PNT))
   {
      sg->out.PNT = AiShaderEvalParamPnt(p_default_value);
   }
}
