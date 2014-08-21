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
   AiParameterStr("enable_attr", "");
   AiParameterBool("default_enable", true);
   AiParameterStr("color_attr", "");
   AiParameterRGBA("default_color", 0.0f, 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "enable_attr", "linkable", false);
   AiMetaDataSetBool(mds, "color_attr", "linkable", false);
}

struct HoldoutData
{
   const char *enable_attr;
   bool default_enable_is_linked;
   bool default_enable;
   
   const char *color_attr;
   bool default_color_is_linked;
   AtRGBA default_color;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(HoldoutData)));
}

node_update
{
   HoldoutData *data = (HoldoutData*) AiNodeGetLocalData(node);
   
   const char *attr_name;
   
   attr_name = AiNodeGetStr(node, "enable_attr");
   data->enable_attr = (strlen(attr_name) > 0 ? attr_name : 0);
   
   data->default_enable_is_linked = AiNodeIsLinked(node, "default_enable");
   if (!data->default_enable_is_linked)
   {
      data->default_enable = AiNodeGetBool(node, "default_enable");
   }
   
   attr_name = AiNodeGetStr(node, "color_attr");
   data->color_attr = (strlen(attr_name) > 0 ? attr_name : 0);
   
   data->default_color_is_linked = AiNodeIsLinked(node, "default_color");
   if (!data->default_color_is_linked)
   {
      data->default_color = AiNodeGetRGBA(node, "default_color");
   }
   
   AiNodeSetLocalData(node, data);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   HoldoutData *data = (HoldoutData*) AiNodeGetLocalData(node);
   
   bool holdout = false;
   
   if ((sg->Rt & AI_RAY_CAMERA) != 0)
   {
      if (!data->enable_attr || !AiUDataGetBool(data->enable_attr, &holdout))
      {
         holdout = (data->default_enable_is_linked ? AiShaderEvalParamBool(p_default_enable) : data->default_enable);
      }
   }
   
   if (holdout)
   {
      if (!data->color_attr || !AiUDataGetRGBA(data->color_attr, &(sg->out.RGBA)))
      {
         sg->out.RGBA = (data->default_color_is_linked ? AiShaderEvalParamRGBA(p_default_color) : data->default_color);
      }
   }
   else
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}

