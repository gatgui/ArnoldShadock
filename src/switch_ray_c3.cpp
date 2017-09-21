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

AI_SHADER_NODE_EXPORT_METHODS(SwitchRayC3Mtd);

enum SwitchRayC3Params
{
   p_camera = 0,
   p_camera_use_default,
   p_shadow,
   p_shadow_use_default,
   p_diffuse_transmit,
   p_diffuse_transmit_use_default,
   p_specular_transmit,
   p_specular_transmit_use_default,
   p_volume,
   p_volume_use_default,
   p_diffuse_reflect,
   p_diffuse_reflect_use_default,
   p_specular_reflect,
   p_specular_reflect_use_default,
   p_subsurface,
   p_subsurface_use_default,
   p_default,
   p_failed
};


node_parameters
{
   AiParameterRGB(SSTR::camera, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::camera_use_default, true);
   AiParameterRGB(SSTR::shadow, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::shadow_use_default, true);
   AiParameterRGB(SSTR::diffuse_transmit, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::diffuse_transmit_use_default, true);
   AiParameterRGB(SSTR::specular_transmit, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::specular_transmit_use_default, true);
   AiParameterRGB(SSTR::volume, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::volume_use_default, true);
   AiParameterRGB(SSTR::diffuse_reflect, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::diffuse_reflect_use_default, true);
   AiParameterRGB(SSTR::specular_reflect, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::specular_reflect_use_default, true);
   AiParameterRGB(SSTR::subsurface, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::subsurface_use_default, true);
   AiParameterRGB(SSTR::_default, 0.0f, 0.0f, 0.0f);
   AiParameterRGB("failed", 0.0f, 0.0f, 0.0f);
}

struct SwitchRayC3Data
{
   bool evalCamera;
   bool cameraUseDefault;
   bool evalShadow;
   bool shadowUseDefault;
   bool evalDiffuseTransmit;
   bool diffuseTransmitUseDefault;
   bool evalSpecularTransmit;
   bool specularTransmitUseDefault;
   bool evalVolume;
   bool volumeUseDefault;
   bool evalDiffuseReflect;
   bool diffuseReflectUseDefault;
   bool evalSpecularReflect;
   bool specularReflectUseDefault;
   bool evalSubsurface;
   bool subsurfaceUseDefault;
};

node_initialize
{
   AiNodeSetLocalData(node, new SwitchRayC3Data());
   AddMemUsage<SwitchRayC3Data>();
}

node_update
{
   SwitchRayC3Data *data = (SwitchRayC3Data*) AiNodeGetLocalData(node);

   data->evalCamera = AiNodeIsLinked(node, SSTR::camera);
   data->cameraUseDefault = AiNodeGetBool(node, SSTR::camera_use_default);
   data->evalShadow = AiNodeIsLinked(node, SSTR::shadow);
   data->shadowUseDefault = AiNodeGetBool(node, SSTR::shadow_use_default);
   data->evalDiffuseTransmit = AiNodeIsLinked(node, SSTR::diffuse_transmit);
   data->diffuseTransmitUseDefault = AiNodeGetBool(node, SSTR::diffuse_transmit_use_default);
   data->evalSpecularTransmit = AiNodeIsLinked(node, SSTR::specular_transmit);
   data->specularTransmitUseDefault = AiNodeGetBool(node, SSTR::specular_transmit_use_default);
   data->evalVolume = AiNodeIsLinked(node, SSTR::volume);
   data->volumeUseDefault = AiNodeGetBool(node, SSTR::volume_use_default);
   data->evalDiffuseReflect = AiNodeIsLinked(node, SSTR::diffuse_reflect);
   data->diffuseReflectUseDefault = AiNodeGetBool(node, SSTR::diffuse_reflect_use_default);
   data->evalSpecularReflect = AiNodeIsLinked(node, SSTR::specular_reflect);
   data->specularReflectUseDefault = AiNodeGetBool(node, SSTR::specular_reflect_use_default);
   data->evalSubsurface = AiNodeIsLinked(node, SSTR::subsurface);
   data->subsurfaceUseDefault = AiNodeGetBool(node, SSTR::subsurface_use_default);
}

node_finish
{
   SwitchRayC3Data *data = (SwitchRayC3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<SwitchRayC3Data>();
}

shader_evaluate
{
   SwitchRayC3Data *data = (SwitchRayC3Data*) AiNodeGetLocalData(node);
   
   if ((sg->Rt & AI_RAY_CAMERA) != 0)
   {
      sg->out.RGB() = AiShaderEvalParamRGB((data->evalCamera || !data->cameraUseDefault) ? p_camera : p_default);
   }
   else if ((sg->Rt & AI_RAY_SHADOW) != 0)
   {
      sg->out.RGB() = AiShaderEvalParamRGB((data->evalShadow || !data->shadowUseDefault) ? p_shadow : p_default);
   }
   else if ((sg->Rt & AI_RAY_DIFFUSE_TRANSMIT) != 0)
   {
      sg->out.RGB() = AiShaderEvalParamRGB((data->evalDiffuseTransmit || !data->diffuseTransmitUseDefault) ? p_diffuse_transmit : p_default);
   }
   else if ((sg->Rt & AI_RAY_SPECULAR_TRANSMIT) != 0)
   {
      sg->out.RGB() = AiShaderEvalParamRGB((data->evalSpecularTransmit || !data->specularTransmitUseDefault) ? p_specular_transmit : p_default);
   }
   else if ((sg->Rt & AI_RAY_VOLUME) != 0)
   {
      sg->out.RGB() = AiShaderEvalParamRGB((data->evalVolume || !data->volumeUseDefault) ? p_volume : p_default);
   }
   else if ((sg->Rt & AI_RAY_DIFFUSE_REFLECT) != 0)
   {
      sg->out.RGB() = AiShaderEvalParamRGB((data->evalDiffuseReflect || !data->diffuseReflectUseDefault) ? p_diffuse_reflect : p_default);
   }
   else if ((sg->Rt & AI_RAY_SPECULAR_REFLECT) != 0)
   {
      sg->out.RGB() = AiShaderEvalParamRGB((data->evalSpecularReflect || !data->specularReflectUseDefault) ? p_specular_reflect : p_default);
   }
   else if ((sg->Rt & AI_RAY_SUBSURFACE) != 0)
   {
      sg->out.RGB() = AiShaderEvalParamRGB((data->evalSubsurface || !data->subsurfaceUseDefault) ? p_subsurface : p_default);
   }
   else
   {
      sg->out.RGB() = AiShaderEvalParamRGB(p_failed);
   }
}
