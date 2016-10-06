#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrSMtd);

enum ShapeAttrSParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterStr(SSTR::_default, "");
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
   
   if (!AiUDataGetStr(data->attribute, &(sg->out.STR)))
   {
      sg->out.STR = AiShaderEvalParamStr(p_default);
   }
}
