#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVP2Mtd);

enum ReadAOVP2Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterPnt2("default_value", 0.0f, 0.0f);
}

struct ReadAOVP2Data
{
   AtString aovName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadAOVP2Data());
   AddMemUsage<ReadAOVP2Data>();
}

node_update
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadAOVP2Data>();
}

shader_evaluate
{
   ReadAOVP2Data *data = (ReadAOVP2Data*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetPnt2(sg, data->aovName, sg->out.PNT2))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default_value);
   }
}
