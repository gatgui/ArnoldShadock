#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVC4Mtd);

enum ReadAOVC4Params
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr("aov_name", "");
   AiParameterRGBA("default_value", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiMetaDataSetBool(mds, "aov_name", "linkable", false);
}

struct ReadAOVC4Data
{
   const char *aovName;
};

node_initialize
{
   ReadAOVC4Data *data = (ReadAOVC4Data*) AiMalloc(sizeof(ReadAOVC4Data));
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVC4Data *data = (ReadAOVC4Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, "aov_name");
}

node_finish
{
   ReadAOVC4Data *data = (ReadAOVC4Data*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   ReadAOVC4Data *data = (ReadAOVC4Data*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetRGBA(sg, data->aovName, sg->out.RGBA))
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_default_value);
   }
}
