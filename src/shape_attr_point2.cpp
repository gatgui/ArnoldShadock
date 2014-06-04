#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrPoint2Mtd);

enum ShapeAttrPoint2Params
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr("attribute", "");
   AiParameterPnt2("default", 0.0f, 0.0f);
   
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
   
   if (!AiUDataGetPnt2(attr_name, &(sg->out.PNT2)))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default);
   }
}
