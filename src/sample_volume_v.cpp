#include "common.h"

#ifdef ARNOLD_4_2_AND_ABOVE

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeVMtd);

enum SampleVolumeVParams
{
   p_field = 0
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
   AiParameterVec("default", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "field", "linkable", false);
   AiMetaDataSetBool(mds, "interpolation", "linkable", false);
}

struct NodeData
{
   const char *field;
   int interpolation;
   bool default_linked;
   AtVector _default;
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
      data->_default = AiNodeGetVec(node, "default");
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   if (!AiVolumeSampleVec(data->field, data->interpolation, &(sg->out.VEC)))
   {
      sg->out.VEC = (data->default_linked ? AiShaderEvalParamVec(p_default) : data->_default);
   }
}

#endif
