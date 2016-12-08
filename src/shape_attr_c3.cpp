#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrC3Mtd);

enum ShapeAttrC3Params
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterRGB(SSTR::_default, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::output_mode, AM_V, AttributeModeNames);
}

struct ShapeAttrC3Data
{
   AtString attribute;
   AttributeMode output_mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrC3Data());
   AddMemUsage<ShapeAttrC3Data>();
}

node_update
{
   ShapeAttrC3Data *data = (ShapeAttrC3Data*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->output_mode = (AttributeMode) AiNodeGetInt(node, SSTR::output_mode);
}

node_finish
{
   ShapeAttrC3Data *data = (ShapeAttrC3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrC3Data>();
}

shader_evaluate
{
   ShapeAttrC3Data *data = (ShapeAttrC3Data*) AiNodeGetLocalData(node);
   
   sg->out.RGB = AI_RGB_BLACK;

   if (data->output_mode == AM_V)
   {
      if (!AiUDataGetRGB(data->attribute, &(sg->out.RGB)) &&
          !AiUDataGetRGBA(data->attribute, &(sg->out.RGBA)))
      {
         sg->out.RGB = AiShaderEvalParamRGB(p_default);
      }
   }
   else
   {
      AtRGBA dVdx, dVdy;
      
      if (!AiUDataGetDxyDerivativesRGB(data->attribute, &dVdx.rgb(), &dVdy.rgb()) &&
          !AiUDataGetDxyDerivativesRGBA(data->attribute, &dVdx, &dVdy))
      {
         sg->out.RGB = AiShaderEvalParamRGB(p_default);
      }
      else
      {
         sg->out.RGBA = (data->output_mode == AM_dVdx ? dVdx : dVdy);
      }
   }
}
