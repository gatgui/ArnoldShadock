#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrP2Mtd);

enum ShapeAttrP2Params
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterPnt2(SSTR::_default, 0.0f, 0.0f);
   
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
   
   sg->out.PNT2 = AI_P2_ZERO;
   
   if (!AiUDataGetPnt2(data->attribute, &(sg->out.PNT2)))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default);
   }
}
