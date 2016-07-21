#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrVMtd);

enum ShapeAttrVParams
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterVec(SSTR::_default, 0.0f, 0.0f, 0.0f);
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
   
   sg->out.VEC = AI_V3_ZERO;

   if (data->output_mode == AM_V)
   {
      if (!AiUDataGetVec(data->attribute, &(sg->out.VEC)) &&
          !AiUDataGetPnt(data->attribute, &(sg->out.PNT)))
      {
         sg->out.VEC = AiShaderEvalParamVec(p_default);
      }
   }
   else
   {
      AtVector dVdx, dVdy;
      
      if (!AiUDataGetDxyDerivativesVec(data->attribute, &dVdx, &dVdy) &&
          !AiUDataGetDxyDerivativesPnt(data->attribute, &dVdx, &dVdy))
      {
         sg->out.VEC = AiShaderEvalParamVec(p_default);
      }
      else
      {
         sg->out.VEC = (data->output_mode == AM_dVdx ? dVdx : dVdy);
      }
   }
}
