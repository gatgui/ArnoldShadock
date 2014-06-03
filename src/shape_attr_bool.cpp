#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrBoolMtd);

enum ShapeAttrBoolParams
{
   p_name,
   p_default
};

node_parameters
{
   AiParameterStr("name", "");
   AiParameterBool("default", false);
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
   
   if (!AiUDataGetBool(attr_name, &(sg->out.BOOL)))
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default);
   }
}
