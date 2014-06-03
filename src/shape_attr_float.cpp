#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrFloatMtd);

enum ShapeAttrFloatParams
{
   p_name,
   p_default
};

node_parameters
{
   AiParameterStr("name", "");
   AiParameterFlt("default", 0.0f);
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
   
   if (!AiUDataGetFlt(attr_name, &(sg->out.FLT)))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default);
   }
}
