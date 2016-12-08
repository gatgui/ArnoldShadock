#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVC4Mtd);

enum ReadAOVC4Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterRGBA("default_value", 0.0f, 0.0f, 0.0f, 1.0f);
}

struct ReadAOVC4Data
{
   AtString aovName;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReadAOVC4Data());
   AddMemUsage<ReadAOVC4Data>();
}

node_update
{
   ReadAOVC4Data *data = (ReadAOVC4Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVC4Data *data = (ReadAOVC4Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReadAOVC4Data>();
}

shader_evaluate
{
   ReadAOVC4Data *data = (ReadAOVC4Data*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetRGBA(sg, data->aovName, sg->out.RGBA))
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_default_value);
   }
}
