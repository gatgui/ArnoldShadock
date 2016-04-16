#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrBMtd);

enum ShapeAttrBParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterBool(SSTR::_default, false);
   
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

   if (!AiUDataGetBool(data->attribute, &(sg->out.BOOL)))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default);
   }
}
