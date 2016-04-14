#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeC3Mtd);

enum SampleVolumeC3Params
{
   p_field = 0,
   p_interpolation,
   p_default
};

enum VolumeInterp
{
   VI_closest = 0,
   VI_linear,
   VI_cubic
};

static const char* VolumeInterpNames[] = {"closest", "linear", "cubic", 0};

node_parameters
{
   AiParameterStr("field", "");
   AiParameterEnum("interpolation", VI_linear, VolumeInterpNames);
   AiParameterRGB("default", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "field", "linkable", false);
   AiMetaDataSetBool(mds, "interpolation", "linkable", false);
}

struct NodeData
{
   const char *field;
   int interpolation;
   bool default_linked;
   AtColor _default;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->field = AiNodeGetStr(node, "field");
   data->interpolation = AiNodeGetInt(node, "interpolation");
   data->default_linked = AiNodeIsLinked(node, "default");
   if (!data->default_linked)
   {
      data->_default = AiNodeGetRGB(node, "default");
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   if (!AiVolumeSampleRGB(data->field, data->interpolation, &(sg->out.RGB)))
   {
      sg->out.RGB = (data->default_linked ? AiShaderEvalParamRGB(p_default) : data->_default);
   }
}
