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
}

struct ShapeAttrBData
{
   AtString attribute;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrBData());
   AddMemUsage<ShapeAttrBData>();
}

node_update
{
   ShapeAttrBData *data = (ShapeAttrBData*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
}

node_finish
{
   ShapeAttrBData *data = (ShapeAttrBData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrBData>();
}

shader_evaluate
{
   ShapeAttrBData *data = (ShapeAttrBData*) AiNodeGetLocalData(node);

   if (!AiUDataGetBool(data->attribute, &(sg->out.BOOL)))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default);
   }
}
