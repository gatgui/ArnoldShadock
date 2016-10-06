#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrIMtd);

enum ShapeAttrIParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterInt(SSTR::_default, 0);
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
   
   if (!AiUDataGetInt(data->attribute, &(sg->out.INT)))
   {
      AtByte b = 0;
      unsigned int ui = 0;

      if (AiUDataGetByte(data->attribute, &b))
      {
         sg->out.INT = int(b);
      }
      else if (AiUDataGetUInt(data->attribute, &ui))
      {
         sg->out.INT = int(ui);
      }
      else
      {
         sg->out.INT = AiShaderEvalParamInt(p_default);
      }
   }
}
