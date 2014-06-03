#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrVectorMtd);

enum ShapeAttrVectorParams
{
   p_name,
   p_default
};

node_parameters
{
   AiParameterStr("name", "");
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
   const char *attr_name = AiShaderEvalParamStr(p_name);
   
   if (!AiUDataGetVec(attr_name, &(sg->out.VEC)) &&
       !AiUDataGetPnt(attr_name, &(sg->out.PNT)))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default);
   }
}
