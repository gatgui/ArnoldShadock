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
   AiParameterFlt(SSTR::camera, 0.0f);
   AiParameterBool(SSTR::camera_use_default, true);
   AiParameterFlt(SSTR::shadow, 0.0f);
   AiParameterBool(SSTR::shadow_use_default, true);
   AiParameterFlt(SSTR::reflected, 0.0f);
   AiParameterBool(SSTR::reflected_use_default, true);
   AiParameterFlt(SSTR::refracted, 0.0f);
   AiParameterBool(SSTR::refracted_use_default, true);
   AiParameterFlt(SSTR::subsurface, 0.0f);
   AiParameterBool(SSTR::subsurface_use_default, true);
   AiParameterFlt(SSTR::diffuse, 0.0f);
   AiParameterBool(SSTR::diffuse_use_default, true);
   AiParameterFlt(SSTR::glossy, 0.0f);
   AiParameterBool(SSTR::glossy_use_default, true);
   AiParameterFlt(SSTR::_default, 0.0f);
   AiParameterFlt("failed", 0.0f);
   
}

struct SwitchRayFData
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
   AiNodeSetLocalData(node, new SwitchRayFData());
   AddMemUsage<SwitchRayFData>();
}

node_update
{
   SwitchRayFData *data = (SwitchRayFData*) AiNodeGetLocalData(node);
   
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
   SwitchRayFData *data = (SwitchRayFData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<SwitchRayFData>();
}

shader_evaluate
{
   SwitchRayFData *data = (SwitchRayFData*) AiNodeGetLocalData(node);
   
   if ((sg->Rt & AI_RAY_CAMERA) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->evalCamera || !data->cameraUseDefault) ? p_camera : p_default);
   }
   else if ((sg->Rt & AI_RAY_SHADOW) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->evalShadow || !data->shadowUseDefault) ? p_shadow : p_default);
   }
   else if ((sg->Rt & AI_RAY_REFLECTED) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->evalReflected || !data->reflectedUseDefault) ? p_reflected : p_default);
   }
   else if ((sg->Rt & AI_RAY_REFRACTED) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->evalRefracted || !data->refractedUseDefault) ? p_refracted : p_default);
   }
   else if ((sg->Rt & AI_RAY_SUBSURFACE) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->evalSubsurface || !data->subsurfaceUseDefault) ? p_subsurface : p_default);
   }
   else if ((sg->Rt & AI_RAY_DIFFUSE) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->evalDiffuse || !data->diffuseUseDefault) ? p_diffuse : p_default);
   }
   else if ((sg->Rt & AI_RAY_GLOSSY) != 0)
   {
      sg->out.FLT = AiShaderEvalParamFlt((data->evalGlossy || !data->glossyUseDefault) ? p_glossy : p_default);
   }
   else
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_failed);
   }
}
