#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SwitchRayFMtd);

enum SwitchRayFParams
{
   p_camera = 0,
   p_camera_use_default,
   p_shadow,
   p_shadow_use_default,
   p_reflected,
   p_reflected_use_default,
   p_refracted,
   p_refracted_use_default,
   #ifdef HAS_SS_RAY
   p_subsurface,
   p_subsurface_use_default,
   #endif
   p_diffuse,
   p_diffuse_use_default,
   p_glossy,
   p_glossy_use_default,
   p_default,
   p_failed
};

node_parameters
{
   AiParameterFlt("camera", 0.0f);
   AiParameterBool("camera_use_default", true);
   AiParameterFlt("shadow", 0.0f);
   AiParameterBool("shadow_use_default", true);
   AiParameterFlt("reflected", 0.0f);
   AiParameterBool("reflected_use_default", true);
   AiParameterFlt("refracted", 0.0f);
   AiParameterBool("refracted_use_default", true);
   #ifdef HAS_SS_RAY
   AiParameterFlt("subsurface", 0.0f);
   AiParameterBool("subsurface_use_default", true);
   #endif
   AiParameterFlt("diffuse", 0.0f);
   AiParameterBool("diffuse_use_default", true);
   AiParameterFlt("glossy", 0.0f);
   AiParameterBool("glossy_use_default", true);
   AiParameterFlt("default", 0.0f);
   AiParameterFlt("failed", 0.0f);
   
   AiMetaDataSetBool(mds, "camera_use_default", "linkable", false);
   AiMetaDataSetBool(mds, "shadow_use_default", "linkable", false);
   AiMetaDataSetBool(mds, "reflected_use_default", "linkable", false);
   AiMetaDataSetBool(mds, "refracted_use_default", "linkable", false);
   #ifdef HAS_SS_RAY
   AiMetaDataSetBool(mds, "subsurface_use_default", "linkable", false);
   #endif
   AiMetaDataSetBool(mds, "diffuse_use_default", "linkable", false);
   AiMetaDataSetBool(mds, "glossy_use_default", "linkable", false);
   
}

struct NodeData
{
   bool camera_linked;
   bool camera_use_default;
   bool shadow_linked;
   bool shadow_use_default;
   bool reflected_linked;
   bool reflected_use_default;
   bool refracted_linked;
   bool refracted_use_default;
   #ifdef HAS_SS_RAY
   bool subsurface_linked;
   bool subsurface_use_default;
   #endif
   bool diffuse_linked;
   bool diffuse_use_default;
   bool glossy_linked;
   bool glossy_use_default;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->camera_linked = AiNodeIsLinked(node, "camera");
   data->camera_use_default = AiNodeGetBool(node, "camera_use_default");
   
   data->shadow_linked = AiNodeIsLinked(node, "shadow");
   data->shadow_use_default = AiNodeGetBool(node, "shadow_use_default");
   
   data->reflected_linked = AiNodeIsLinked(node, "reflected");
   data->reflected_use_default = AiNodeGetBool(node, "reflected_use_default");
   
   data->refracted_linked = AiNodeIsLinked(node, "refracted");
   data->refracted_use_default = AiNodeGetBool(node, "refracted_use_default");
   
   #ifdef HAS_SS_RAY
   data->subsurface_linked = AiNodeIsLinked(node, "subsurface");
   data->subsurface_use_default = AiNodeGetBool(node, "subsurface_use_default");
   #endif
   
   data->diffuse_linked = AiNodeIsLinked(node, "diffuse");
   data->diffuse_use_default = AiNodeGetBool(node, "diffuse_use_default");
   
   data->glossy_linked = AiNodeIsLinked(node, "glossy");
   data->glossy_use_default = AiNodeGetBool(node, "glossy_use_default");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   if ((sg->Rt & AI_RAY_CAMERA) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->camera_linked || !data->camera_use_default) ? p_camera : p_default);
   }
   else if ((sg->Rt & AI_RAY_SHADOW) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->shadow_linked || !data->shadow_use_default) ? p_shadow : p_default);
   }
   else if ((sg->Rt & AI_RAY_REFLECTED) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->reflected_linked || !data->reflected_use_default) ? p_reflected : p_default);
   }
   else if ((sg->Rt & AI_RAY_REFRACTED) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->refracted_linked || !data->refracted_use_default) ? p_refracted : p_default);
   }
   #ifdef HAS_SS_RAY
   else if ((sg->Rt & AI_RAY_SUBSURFACE) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->subsurface_linked || !data->subsurface_use_default) ? p_subsurface : p_default);
   }
   #endif
   else if ((sg->Rt & AI_RAY_DIFFUSE) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->diffuse_linked || !data->diffuse_use_default) ? p_diffuse : p_default);
   }
   else if ((sg->Rt & AI_RAY_GLOSSY) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->glossy_linked || !data->glossy_use_default) ? p_glossy : p_default);
   }
   else
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_failed);
   }
}
