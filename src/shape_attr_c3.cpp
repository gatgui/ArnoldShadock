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

struct NodeData
{
   AtString attribute;
   AttributeMode output_mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->output_mode = (AttributeMode) AiNodeGetInt(node, SSTR::output_mode);
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
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
