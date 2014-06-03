#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrRGBAMtd);

enum ShapeAttrRGBAParams
{
   p_name,
   p_default
};

node_parameters
{
   AiParameterStr("name", "");
   AiParameterRGBA("default", 0.0f, 0.0f, 0.0f, 1.0f);
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
   const char *attr_name = AiShaderEvalParamStr(p_name);
   
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
