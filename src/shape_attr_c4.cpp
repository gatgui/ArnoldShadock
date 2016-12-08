#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrC4Mtd);

enum ShapeAttrC4Params
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterRGBA(SSTR::_default, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterEnum(SSTR::output_mode, AM_V, AttributeModeNames);
}

struct ShapeAttrC4Data
{
   AtString attribute;
   AttributeMode output_mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrC4Data());
   AddMemUsage<ShapeAttrC4Data>();
}

node_update
{
   ShapeAttrC4Data *data = (ShapeAttrC4Data*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->output_mode = (AttributeMode) AiNodeGetInt(node, SSTR::output_mode);
}

node_finish
{
   ShapeAttrC4Data *data = (ShapeAttrC4Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrC4Data>();
}

shader_evaluate
{
   ShapeAttrC4Data *data = (ShapeAttrC4Data*) AiNodeGetLocalData(node);

   sg->out.RGBA = AI_RGBA_BLACK;
   
   if (data->output_mode == AM_V)
   {
      if (!AiUDataGetRGBA(data->attribute, &(sg->out.RGBA)))
      {
         if (!AiUDataGetRGB(data->attribute, &(sg->out.RGB)))
         {
            sg->out.RGBA = AiShaderEvalParamRGBA(p_default);
         }
         else
         {
            sg->out.RGBA.a = 1.0f;
         }
      }
   }
   else
   {
      AtRGBA dVdx, dVdy;
      
      if (!AiUDataGetDxyDerivativesRGBA(data->attribute, &dVdx, &dVdy))
      {
         if (!AiUDataGetDxyDerivativesRGB(data->attribute, &dVdx.rgb(), &dVdy.rgb()))
         {
            sg->out.RGBA = AiShaderEvalParamRGBA(p_default);
            return;
         }
         
         dVdx.a = 1.0f;
         dVdy.a = 1.0f;
      }
      
      sg->out.RGBA = (data->output_mode == AM_dVdx ? dVdx : dVdy);
   }
}
