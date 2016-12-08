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

struct ShapeAttrSData
{
   AtString attribute;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrSData());
   AddMemUsage<ShapeAttrSData>();
}

node_update
{
   ShapeAttrSData *data = (ShapeAttrSData*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
}

node_finish
{
   ShapeAttrSData *data = (ShapeAttrSData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrSData>();
}

shader_evaluate
{
   ShapeAttrSData *data = (ShapeAttrSData*) AiNodeGetLocalData(node);
   
   if (!AiUDataGetStr(data->attribute, &(sg->out.STR)))
   {
      sg->out.STR = AiShaderEvalParamStr(p_default);
   }
}
