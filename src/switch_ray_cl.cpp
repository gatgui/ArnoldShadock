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

AI_SHADER_NODE_EXPORT_METHODS(SwitchRayClMtd);

enum SwitchRayClParams
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
   AiParameterClosure(SSTR::camera);
   AiParameterBool(SSTR::camera_use_default, true);
   AiParameterClosure(SSTR::shadow);
   AiParameterBool(SSTR::shadow_use_default, true);
   AiParameterClosure(SSTR::diffuse_transmit);
   AiParameterBool(SSTR::diffuse_transmit_use_default, true);
   AiParameterClosure(SSTR::specular_transmit);
   AiParameterBool(SSTR::specular_transmit_use_default, true);
   AiParameterClosure(SSTR::volume);
   AiParameterBool(SSTR::volume_use_default, true);
   AiParameterClosure(SSTR::diffuse_reflect);
   AiParameterBool(SSTR::diffuse_reflect_use_default, true);
   AiParameterClosure(SSTR::specular_reflect);
   AiParameterBool(SSTR::specular_reflect_use_default, true);
   AiParameterClosure(SSTR::subsurface);
   AiParameterBool(SSTR::subsurface_use_default, true);
   AiParameterClosure(SSTR::_default);
   AiParameterClosure("failed");
}

struct SwitchRayClData
{
   bool cameraUseDefault;
   bool shadowUseDefault;
   bool diffuseTransmitUseDefault;
   bool specularTransmitUseDefault;
   bool volumeUseDefault;
   bool diffuseReflectUseDefault;
   bool specularReflectUseDefault;
   bool subsurfaceUseDefault;
};

node_initialize
{
   AiNodeSetLocalData(node, new SwitchRayClData());
   AddMemUsage<SwitchRayClData>();
}

node_update
{
   SwitchRayClData *data = (SwitchRayClData*) AiNodeGetLocalData(node);
   
   data->cameraUseDefault = AiNodeGetBool(node, SSTR::camera_use_default);
   data->shadowUseDefault = AiNodeGetBool(node, SSTR::shadow_use_default);
   data->diffuseTransmitUseDefault = AiNodeGetBool(node, SSTR::diffuse_transmit_use_default);
   data->specularTransmitUseDefault = AiNodeGetBool(node, SSTR::specular_transmit_use_default);
   data->volumeUseDefault = AiNodeGetBool(node, SSTR::volume_use_default);
   data->diffuseReflectUseDefault = AiNodeGetBool(node, SSTR::diffuse_reflect_use_default);
   data->specularReflectUseDefault = AiNodeGetBool(node, SSTR::specular_reflect_use_default);
   data->subsurfaceUseDefault = AiNodeGetBool(node, SSTR::subsurface_use_default);
}

node_finish
{
   SwitchRayClData *data = (SwitchRayClData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<SwitchRayClData>();
}

shader_evaluate
{
   SwitchRayClData *data = (SwitchRayClData*) AiNodeGetLocalData(node);
   
   if ((sg->Rt & AI_RAY_CAMERA) != 0)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(!data->cameraUseDefault ? p_camera : p_default);
   }
   else if ((sg->Rt & AI_RAY_SHADOW) != 0)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(!data->shadowUseDefault ? p_shadow : p_default);
   }
   else if ((sg->Rt & AI_RAY_DIFFUSE_TRANSMIT) != 0)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(!data->diffuseTransmitUseDefault ? p_diffuse_transmit : p_default);
   }
   else if ((sg->Rt & AI_RAY_SPECULAR_TRANSMIT) != 0)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(!data->specularTransmitUseDefault ? p_specular_transmit : p_default);
   }
   else if ((sg->Rt & AI_RAY_VOLUME) != 0)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(!data->volumeUseDefault ? p_volume : p_default);
   }
   else if ((sg->Rt & AI_RAY_DIFFUSE_REFLECT) != 0)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(!data->diffuseReflectUseDefault ? p_diffuse_reflect : p_default);
   }
   else if ((sg->Rt & AI_RAY_SPECULAR_REFLECT) != 0)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(!data->specularReflectUseDefault ? p_specular_reflect : p_default);
   }
   else if ((sg->Rt & AI_RAY_SUBSURFACE) != 0)
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(!data->subsurfaceUseDefault ? p_subsurface : p_default);
   }
   else
   {
      sg->out.CLOSURE() = AiShaderEvalParamClosure(p_failed);
   }
}
