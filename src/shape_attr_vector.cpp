#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrVectorMtd);

enum ShapeAttrVectorParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr("attribute", "");
   AiParameterVec("default", 0.0f, 0.0f, 0.0f);
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
   
   if (!AiUDataGetVec(attr_name, &(sg->out.VEC)) &&
       !AiUDataGetPnt(attr_name, &(sg->out.PNT)))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default);
   }
}
