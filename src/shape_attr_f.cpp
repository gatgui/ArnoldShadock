#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrFMtd);

enum ShapeAttrFParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterFlt(SSTR::_default, 0.0f);
   
   AiMetaDataSetBool(mds, SSTR::attribute, SSTR::linkable, false);
}

struct NodeData
{
   AtString attribute;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
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
   
   if (!AiUDataGetFlt(data->attribute, &(sg->out.FLT)))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default);
   }
}
