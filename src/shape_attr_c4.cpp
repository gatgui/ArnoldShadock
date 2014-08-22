#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrC4Mtd);

enum ShapeAttrC4Params
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr("attribute", "");
   AiParameterRGBA("default", 0.0f, 0.0f, 0.0f, 1.0f);
   
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
   
   if (!AiUDataGetRGBA(attr_name, &(sg->out.RGBA)))
   {
      if (AiUDataGetRGB(attr_name, &(sg->out.RGB)))
      {
         sg->out.RGBA.a = 1.0f;
      }
      else
      {
         sg->out.RGBA = AiShaderEvalParamRGBA(p_default);
      }
   }
}
