#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrC3Mtd);

enum ShapeAttrC3Params
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterRGB(SSTR::_default, 0.0f, 0.0f, 0.0f);
   
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
   
   sg->out.RGB = AI_RGB_BLACK;

   if (!AiUDataGetRGB(data->attribute, &(sg->out.RGB)) &&
       !AiUDataGetRGBA(data->attribute, &(sg->out.RGBA)))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default);
   }
}
