#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrColorMtd);

enum ShapeAttrColorParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr("attribute", "");
   AiParameterRGB("default", 0.0f, 0.0f, 0.0f);
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
   
   if (!AiUDataGetRGB(attr_name, &(sg->out.RGB)) &&
       !AiUDataGetRGBA(attr_name, &(sg->out.RGBA)))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_default);
   }
}
