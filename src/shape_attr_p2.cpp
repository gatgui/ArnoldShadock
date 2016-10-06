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
