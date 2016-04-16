#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeC3Mtd);

enum SampleVolumeC3Params
{
   p_field = 0,
   p_interpolation,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::field, "");
   AiParameterEnum(SSTR::interpolation, VI_linear, VolumeInterpNames);
   AiParameterRGB(SSTR::_default, 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, SSTR::field, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::interpolation, SSTR::linkable, false);
}

struct NodeData
{
   AtString field;
   int interpolation;
   bool evalDefault;
   AtColor _default;
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
      data->_default = AiNodeGetRGB(node, SSTR::_default);
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
   
   if (!AiVolumeSampleRGB(data->field, data->interpolation, &(sg->out.RGB)))
   {
      sg->out.RGB = (data->evalDefault ? AiShaderEvalParamRGB(p_default) : data->_default);
   }
}
