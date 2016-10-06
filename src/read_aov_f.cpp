#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReadAOVFMtd);

enum ReadAOVFParams
{
   p_aov_name = 0,
   p_default_value
};

node_parameters
{
   AiParameterStr(SSTR::aov_name, "");
   AiParameterFlt("default_value", 0.0f);
}

struct ReadAOVFData
{
   AtString aovName;
};

node_initialize
{
   ReadAOVFData *data = (ReadAOVFData*) AiMalloc(sizeof(ReadAOVFData));
   AiAddMemUsage(AtInt64(sizeof(ReadAOVFData)), "shading_blocks");
   AiNodeSetLocalData(node, data);
}

node_update
{
   ReadAOVFData *data = (ReadAOVFData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
}

node_finish
{
   ReadAOVFData *data = (ReadAOVFData*) AiNodeGetLocalData(node);
   AiFree(data);
   AiAddMemUsage(-AtInt64(sizeof(ReadAOVFData)), "shading_blocks");
}

shader_evaluate
{
   ReadAOVFData *data = (ReadAOVFData*) AiNodeGetLocalData(node);
   
   if (!AiAOVGetFlt(sg, data->aovName, sg->out.FLT))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default_value);
   }
}
