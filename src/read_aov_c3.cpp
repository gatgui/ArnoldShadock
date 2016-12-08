#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVC3Mtd);

enum ReadAOVC3Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterRGB("default_value", 0.0f, 0.0f, 0.0f);
}

struct ReadAOVC3Data
{
   AtString aovName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadAOVC3Data());
   AddMemUsage<ReadAOVC3Data>();
}

node_update
{
   ReadAOVC3Data *data = (ReadAOVC3Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVC3Data *data = (ReadAOVC3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadAOVC3Data>();
}

shader_evaluate
{
   ReadAOVC3Data *data = (ReadAOVC3Data*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetRGB(sg, data->aovName, sg->out.RGB))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
   }
}
