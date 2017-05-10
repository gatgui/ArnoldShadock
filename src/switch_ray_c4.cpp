/*
MIT License
Copyright (c) 2014~ Gaetan Guidet
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SwitchRayC4Mtd);

enum SwitchRayC4Params
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
   AiParameterRGBA(SSTR::camera, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::camera_use_default, true);
   AiParameterRGBA(SSTR::shadow, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::shadow_use_default, true);
   AiParameterRGBA(SSTR::reflected, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::reflected_use_default, true);
   AiParameterRGBA(SSTR::refracted, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::refracted_use_default, true);
   AiParameterRGBA(SSTR::subsurface, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::subsurface_use_default, true);
   AiParameterRGBA(SSTR::diffuse, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::diffuse_use_default, true);
   AiParameterRGBA(SSTR::glossy, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::glossy_use_default, true);
   AiParameterRGBA(SSTR::_default, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterRGBA("failed", 0.0f, 0.0f, 0.0f, 1.0f);
   
}

struct SwitchRayC4Data
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
   AiNodeSetLocalData(node, new SwitchRayC4Data());
   AddMemUsage<SwitchRayC4Data>();
}

node_update
{
   SwitchRayC4Data *data = (SwitchRayC4Data*) AiNodeGetLocalData(node);
   
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
   SwitchRayC4Data *data = (SwitchRayC4Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<SwitchRayC4Data>();
}

shader_evaluate
{
   SwitchRayC4Data *data = (SwitchRayC4Data*) AiNodeGetLocalData(node);
   
   if ((sg->Rt & AI_RAY_CAMERA) != 0)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA((data->evalCamera || !data->cameraUseDefault) ? p_camera : p_default);
   }
   else if ((sg->Rt & AI_RAY_SHADOW) != 0)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA((data->evalShadow || !data->shadowUseDefault) ? p_shadow : p_default);
   }
   else if ((sg->Rt & AI_RAY_REFLECTED) != 0)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA((data->evalReflected || !data->reflectedUseDefault) ? p_reflected : p_default);
   }
   else if ((sg->Rt & AI_RAY_REFRACTED) != 0)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA((data->evalRefracted || !data->refractedUseDefault) ? p_refracted : p_default);
   }
   else if ((sg->Rt & AI_RAY_SUBSURFACE) != 0)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA((data->evalSubsurface || !data->subsurfaceUseDefault) ? p_subsurface : p_default);
   }
   else if ((sg->Rt & AI_RAY_DIFFUSE) != 0)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA((data->evalDiffuse || !data->diffuseUseDefault) ? p_diffuse : p_default);
   }
   else if ((sg->Rt & AI_RAY_GLOSSY) != 0)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA((data->evalGlossy || !data->glossyUseDefault) ? p_glossy : p_default);
   }
   else
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_failed);
   }
}
