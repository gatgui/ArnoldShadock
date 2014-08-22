#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVC3Mtd);

enum ReadAOVC3Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterRGB("default_value", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVC3Data
{
   const char *aovName;
};

node_initialize
{
   ReadAOVC3Data *data = (ReadAOVC3Data*) AiMalloc(sizeof(ReadAOVC3Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVC3Data *data = (ReadAOVC3Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVC3Data *data = (ReadAOVC3Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVC3Data *data = (ReadAOVC3Data*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetRGB(sg, data->aovName, sg->out.RGB))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default_value);
   }
}
