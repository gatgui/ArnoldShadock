#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrP2Mtd);

enum ShapeAttrP2Params
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterPnt2(SSTR::_default, 0.0f, 0.0f);
   AiParameterEnum(SSTR::output_mode, AM_V, AttributeModeNames);
}

struct ShapeAttrP2Data
{
   AtString attribute;
   AttributeMode output_mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrP2Data());
   AddMemUsage<ShapeAttrP2Data>();
}

node_update
{
   ShapeAttrP2Data *data = (ShapeAttrP2Data*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->output_mode = (AttributeMode) AiNodeGetInt(node, SSTR::output_mode);
}

node_finish
{
   ShapeAttrP2Data *data = (ShapeAttrP2Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrP2Data>();
}

shader_evaluate
{
   ShapeAttrP2Data *data = (ShapeAttrP2Data*) AiNodeGetLocalData(node);
   
   sg->out.PNT2 = AI_P2_ZERO;
   
   if (data->output_mode == AM_V)
   {
      if (!AiUDataGetPnt2(data->attribute, &(sg->out.PNT2)))
      {
         sg->out.PNT2 = AiShaderEvalParamPnt2(p_default);
      }
   }
   else
   {
      AtPoint2 dVdx, dVdy;
      
      if (!AiUDataGetDxyDerivativesPnt2(data->attribute, &dVdx, &dVdx))
      {
         sg->out.PNT2 = AiShaderEvalParamPnt2(p_default);
      }
      else
      {
         sg->out.PNT2 = (data->output_mode == AM_dVdx ? dVdx : dVdy);
      }
   }
}
