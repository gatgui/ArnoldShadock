#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SwitchRayC3Mtd);

enum SwitchRayC3Params
{
   p_camera = 0,
   p_camera_use_default,
   p_shadow,
   p_shadow_use_default,
   p_reflected,
   p_reflected_use_default,
   p_refracted,
   p_refracted_use_default,
   p_subsurface,
   p_subsurface_use_default,
   p_diffuse,
   p_diffuse_use_default,
   p_glossy,
   p_glossy_use_default,
   p_default,
   p_failed
};

node_parameters
{
   AiParameterRGB(SSTR::camera, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::camera_use_default, true);
   AiParameterRGB(SSTR::shadow, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::shadow_use_default, true);
   AiParameterRGB(SSTR::reflected, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::reflected_use_default, true);
   AiParameterRGB(SSTR::refracted, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::refracted_use_default, true);
   AiParameterRGB(SSTR::subsurface, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::subsurface_use_default, true);
   AiParameterRGB(SSTR::diffuse, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::diffuse_use_default, true);
   AiParameterRGB(SSTR::glossy, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::glossy_use_default, true);
   AiParameterRGB(SSTR::_default, 0.0f, 0.0f, 0.0f);
   AiParameterRGB("failed", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, SSTR::camera_use_default, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::shadow_use_default, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::reflected_use_default, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::refracted_use_default, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::subsurface_use_default, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::diffuse_use_default, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::glossy_use_default, SSTR::linkable, false);
   
}

struct NodeData
{
   bool evalCamera;
   bool cameraUseDefault;
   bool evalShadow;
   bool shadowUseDefault;
   bool evalReflected;
   bool reflectedUseDefault;
   bool evalRefracted;
   bool refractedUseDefault;
   bool evalSubsurface;
   bool subsurfaceUseDefault;
   bool evalDiffuse;
   bool diffuseUseDefault;
   bool evalGlossy;
   bool glossyUseDefault;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->evalCamera = AiNodeIsLinked(node, SSTR::camera);
   data->cameraUseDefault = AiNodeGetBool(node, SSTR::camera_use_default);
   
   data->evalShadow = AiNodeIsLinked(node, SSTR::shadow);
   data->shadowUseDefault = AiNodeGetBool(node, SSTR::shadow_use_default);
   
   data->evalReflected = AiNodeIsLinked(node, SSTR::reflected);
   data->reflectedUseDefault = AiNodeGetBool(node, SSTR::reflected_use_default);
   
   data->evalRefracted = AiNodeIsLinked(node, SSTR::refracted);
   data->refractedUseDefault = AiNodeGetBool(node, SSTR::refracted_use_default);
   
   data->evalSubsurface = AiNodeIsLinked(node, SSTR::subsurface);
   data->subsurfaceUseDefault = AiNodeGetBool(node, SSTR::subsurface_use_default);
   
   data->evalDiffuse = AiNodeIsLinked(node, SSTR::diffuse);
   data->diffuseUseDefault = AiNodeGetBool(node, SSTR::diffuse_use_default);
   
   data->evalGlossy = AiNodeIsLinked(node, SSTR::glossy);
   data->glossyUseDefault = AiNodeGetBool(node, SSTR::glossy_use_default);
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
   
   if ((sg->Rt & AI_RAY_CAMERA) != 0)
   {
      sg->out.RGB = AiShaderEvalParamRGB((data->evalCamera || !data->cameraUseDefault) ? p_camera : p_default);
   }
   else if ((sg->Rt & AI_RAY_SHADOW) != 0)
   {
      sg->out.RGB = AiShaderEvalParamRGB((data->evalShadow || !data->shadowUseDefault) ? p_shadow : p_default);
   }
   else if ((sg->Rt & AI_RAY_REFLECTED) != 0)
   {
      sg->out.RGB = AiShaderEvalParamRGB((data->evalReflected || !data->reflectedUseDefault) ? p_reflected : p_default);
   }
   else if ((sg->Rt & AI_RAY_REFRACTED) != 0)
   {
      sg->out.RGB = AiShaderEvalParamRGB((data->evalRefracted || !data->refractedUseDefault) ? p_refracted : p_default);
   }
   else if ((sg->Rt & AI_RAY_SUBSURFACE) != 0)
   {
      sg->out.RGB = AiShaderEvalParamRGB((data->evalSubsurface || !data->subsurfaceUseDefault) ? p_subsurface : p_default);
   }
   else if ((sg->Rt & AI_RAY_DIFFUSE) != 0)
   {
      sg->out.RGB = AiShaderEvalParamRGB((data->evalDiffuse || !data->diffuseUseDefault) ? p_diffuse : p_default);
   }
   else if ((sg->Rt & AI_RAY_GLOSSY) != 0)
   {
      sg->out.RGB = AiShaderEvalParamRGB((data->evalGlossy || !data->glossyUseDefault) ? p_glossy : p_default);
   }
   else
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_failed);
   }
}
