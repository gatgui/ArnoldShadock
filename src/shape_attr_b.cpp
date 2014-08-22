#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrBMtd);

enum ShapeAttrBParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr("attribute", "");
   AiParameterBool("default", false);
   
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
   
   if (!AiUDataGetBool(attr_name, &(sg->out.BOOL)))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default);
   }
}
