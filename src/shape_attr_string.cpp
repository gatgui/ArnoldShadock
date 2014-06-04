#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrStringMtd);

enum ShapeAttrStringParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr("attribute", "");
   AiParameterStr("default", "");
   
   AiMetaDataSetBool(mds, "attribute", "linkable", false);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   const char *attr_name = AiShaderEvalParamStr(p_attribute);
   
   if (!AiUDataGetStr(attr_name, &(sg->out.STR)))
   {
      sg->out.STR = AiShaderEvalParamStr(p_default);
   }
}
