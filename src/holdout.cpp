#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(HoldoutMtd);

enum HoldoutParams
{
   p_input = 0,
   p_enable_attr,
   p_default_enable,
   p_color_attr,
   p_default_color
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterStr(SSTR::enable_attr, "");
   AiParameterBool(SSTR::default_enable, true);
   AiParameterStr(SSTR::color_attr, "");
   AiParameterRGBA(SSTR::default_color, 0.0f, 0.0f, 0.0f, 0.0f);
}

struct NodeData
{
   AtString enableAttr;
   bool evalDefaultEnable;
   bool defaultEnable;
   
   AtString colorAttr;
   bool evalDefaultColor;
   AtRGBA defaultColor;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->enableAttr = AiNodeGetStr(node, SSTR::enable_attr);
   
   data->evalDefaultEnable = AiNodeIsLinked(node, SSTR::default_enable);
   if (!data->evalDefaultEnable)
   {
      data->defaultEnable = AiNodeGetBool(node, SSTR::default_enable);
   }
   
   data->colorAttr = AiNodeGetStr(node, SSTR::color_attr);
   
   data->evalDefaultColor = AiNodeIsLinked(node, SSTR::default_color);
   if (!data->evalDefaultColor)
   {
      data->defaultColor = AiNodeGetRGBA(node, SSTR::default_color);
   }
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
   
   bool holdout = false;
   
   if ((sg->Rt & AI_RAY_CAMERA) != 0)
   {
      if (data->enableAttr.empty() || !AiUDataGetBool(data->enableAttr, &holdout))
      {
         holdout = (data->evalDefaultEnable ? AiShaderEvalParamBool(p_default_enable) : data->defaultEnable);
      }
   }
   
   if (holdout)
   {
      if (data->colorAttr.empty() || !AiUDataGetRGBA(data->colorAttr, &(sg->out.RGBA)))
      {
         sg->out.RGBA = (data->evalDefaultColor ? AiShaderEvalParamRGBA(p_default_color) : data->defaultColor);
      }
   }
   else
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}

