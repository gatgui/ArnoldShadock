#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrC4Mtd);

enum ShapeAttrC4Params
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterRGBA(SSTR::_default, 0.0f, 0.0f, 0.0f, 1.0f);
   
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

   sg->out.RGBA = AI_RGBA_BLACK;
   
   if (!AiUDataGetRGBA(data->attribute, &(sg->out.RGBA)))
   {
      if (AiUDataGetRGB(data->attribute, &(sg->out.RGB)))
      {
         sg->out.RGBA.a = 1.0f;
      }
      else
      {
         sg->out.RGBA = AiShaderEvalParamRGBA(p_default);
      }
   }
}
