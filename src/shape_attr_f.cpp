#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrFMtd);

enum ShapeAttrFParams
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterFlt(SSTR::_default, 0.0f);
   AiParameterEnum(SSTR::output_mode, AM_V, AttributeModeNames);
   
   AiMetaDataSetBool(mds, SSTR::attribute, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::output_mode, SSTR::linkable, false);
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
   
   if (data->output_mode == AM_V)
   {
      // Int, UInt, Byte?
      if (!AiUDataGetFlt(data->attribute, &(sg->out.FLT)))
      {
         sg->out.FLT = AiShaderEvalParamFlt(p_default);
      }
   }
   else
   {
      float dVdx = 0.0f;
      float dVdy = 0.0f;
      
      if (!AiUDataGetDxyDerivativesFlt(data->attribute, &dVdx, &dVdx))
      {
         sg->out.FLT = AiShaderEvalParamFlt(p_default);
      }
      else
      {
         sg->out.FLT = (data->output_mode == AM_dVdx ? dVdx : dVdy);
      }
   }
}
