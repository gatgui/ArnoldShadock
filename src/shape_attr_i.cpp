#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrIMtd);

enum ShapeAttrIParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr("attribute", "");
   AiParameterInt("default", 0);
   
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
   
   if (!AiUDataGetInt(attr_name, &(sg->out.INT)))
   {
      if (AiUDataGetByte(attr_name, &(sg->out.BYTE)))
      {
         sg->out.INT = int(sg->out.BYTE);
      }
      else if (AiUDataGetUInt(attr_name, &(sg->out.UINT)))
      {
         sg->out.INT = int(sg->out.UINT);
      }
      else
      {
         sg->out.INT = AiShaderEvalParamInt(p_default);
      }
   }
}
