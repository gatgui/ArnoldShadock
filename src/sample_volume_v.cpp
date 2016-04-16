#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeVMtd);

enum SampleVolumeVParams
{
   p_field = 0,
   p_interpolation,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::field, "");
   AiParameterEnum(SSTR::interpolation, VI_linear, VolumeInterpNames);
   AiParameterVec(SSTR::_default, 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, SSTR::field, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::interpolation, SSTR::linkable, false);
}

struct NodeData
{
   AtString field;
   int interpolation;
   bool evalDefault;
   AtVector _default;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->field = AiNodeGetStr(node, SSTR::field);
   data->interpolation = AiNodeGetInt(node, SSTR::interpolation);
   data->evalDefault = AiNodeIsLinked(node, SSTR::_default);
   if (!data->evalDefault)
   {
      data->_default = AiNodeGetVec(node, SSTR::_default);
   }
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   if (!AiVolumeSampleVec(data->field, data->interpolation, &(sg->out.VEC)))
   {
      sg->out.VEC = (data->evalDefault ? AiShaderEvalParamVec(p_default) : data->_default);
   }
}
