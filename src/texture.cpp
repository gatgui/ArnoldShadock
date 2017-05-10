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

AI_SHADER_NODE_EXPORT_METHODS(TextureMtd);

enum TextureParams
{
   p_filename = 0,
   p_filter,
   p_mipmap_mode,
   p_mipmap_bias,
   p_single_channel,
   p_fill,
   p_start_channel,
   p_swrap,
   p_twrap,
   p_sscale,
   p_tscale,
   p_sflip,
   p_tflip,
   p_swap_st,
   p_swidth,
   p_twidth,
   p_sblur,
   p_tblur,
   p_multiply,
   p_offset,
   p_cache_texture_handles,
   p_use_default_color,
   p_default_color
};

enum Filter
{
   Filter_closest = 0,
   Filter_bilinear,
   Filter_bicubic,
   Filter_smart_bicubic
};

enum MipMode
{
   Mip_default = 0,
   Mip_none,
   Mip_one,
   Mip_trilinear,
   Mip_anisotropic
};

enum Wrap
{
   Wrap_periodic = 0,
   Wrap_black,
   Wrap_clamp,
   Wrap_mirror,
   Wrap_file
};

static const char* FilterNames[] = 
{
   "closest",
   "bilinear",
   "bicubic",
   "smart_bicubic",
   NULL
};

static const char* MipNames[] =
{
   "default",
   "none",
   "one",
   "trilinear",
   "anisotropic",
   NULL
};

static const char* WrapNames[] =
{
   "periodic",
   "black",
   "clamp",
   "mirror",
   "file",
   NULL
};

node_parameters
{
   AiParameterStr(SSTR::filename, "");
   AiParameterEnum(SSTR::filter, Filter_smart_bicubic, FilterNames);
   AiParameterEnum(SSTR::mipmap_mode, Mip_default, MipNames);
   AiParameterInt(SSTR::mipmap_bias, 0);
   AiParameterBool(SSTR::single_channel, false);
   AiParameterFlt(SSTR::fill, 1.0f);
   AiParameterByte("start_channel", 0);
   AiParameterEnum(SSTR::swrap, Wrap_periodic, WrapNames);
   AiParameterEnum(SSTR::twrap, Wrap_periodic, WrapNames);
   AiParameterFlt("sscale", 1.0f);
   AiParameterFlt("tscale", 1.0f);
   AiParameterBool(SSTR::sflip, false);
   AiParameterBool(SSTR::tflip, false);
   AiParameterBool(SSTR::swap_st, false);
   AiParameterFlt(SSTR::swidth, 1.0f);
   AiParameterFlt(SSTR::twidth, 1.0f);
   AiParameterFlt(SSTR::sblur, 0.0f);
   AiParameterFlt(SSTR::tblur, 0.0f);
   AiParameterRGB("multiply", 1.0f, 1.0f, 1.0f);
   AiParameterRGB("offset", 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::cache_texture_handles, true);
   AiParameterBool(SSTR::use_default_color, false);
   AiParameterRGBA(SSTR::default_color, 0.0f, 0.0f, 0.0f, 1.0f);
}

struct TextureData
{
   bool evalPath;
   const char *filename;
   AtTextureHandle *handle;
   AtTextureParams params;
   bool useDefaultColor;
};


node_initialize
{
   TextureData *data = new TextureData();
   AddMemUsage<TextureData>();
   
   // initialize for first node_update call
   data->handle = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   TextureData *data = (TextureData*) AiNodeGetLocalData(node);
   
   data->evalPath = AiNodeIsLinked(node, SSTR::filename);
   data->filename = 0;
   if (data->handle)
   {
      AiTextureHandleDestroy(data->handle);
   }
   data->handle = 0;
   data->useDefaultColor = AiNodeGetBool(node, SSTR::use_default_color);
   AiTextureParamsSetDefaults(&(data->params));
   data->params.filter = AiNodeGetInt(node, SSTR::filter);
   data->params.mipmap_mode = AiNodeGetInt(node, SSTR::mipmap_mode);
   data->params.mipmap_bias = AiNodeGetInt(node, SSTR::mipmap_bias);
   data->params.single_channel = AiNodeGetBool(node, SSTR::single_channel);
   data->params.fill = AiNodeGetFlt(node, SSTR::fill);
   data->params.flip_s = AiNodeGetBool(node, SSTR::sflip);
   data->params.flip_t = AiNodeGetBool(node, SSTR::tflip);
   data->params.swap_st = AiNodeGetBool(node, SSTR::swap_st);
   data->params.wrap_s = AiNodeGetInt(node, SSTR::swrap);
   data->params.wrap_t = AiNodeGetInt(node, SSTR::twrap);
   data->params.width_s = AiNodeGetFlt(node, SSTR::swidth);
   data->params.width_t = AiNodeGetFlt(node, SSTR::twidth);
   data->params.blur_s = AiNodeGetFlt(node, SSTR::sblur);
   data->params.blur_t = AiNodeGetFlt(node, SSTR::tblur);
   
   if (!data->evalPath)
   {
      data->filename = AiNodeGetStr(node, SSTR::filename);
      
      if (AiNodeGetBool(node, SSTR::cache_texture_handles))
      {
         data->handle = AiTextureHandleCreate(data->filename);
         if (!data->handle)
         {
            AiMsgWarning("[texture] File \"%s\" doesn't exist", data->filename);
         }
      }
   }
}

node_finish
{
   TextureData *data = (TextureData*) AiNodeGetLocalData(node);
   
   if (data->handle)
   {
      AiTextureHandleDestroy(data->handle);
   }
   
   delete data;
   SubMemUsage<TextureData>();
}

shader_evaluate
{
   TextureData *data = (TextureData*) AiNodeGetLocalData(node);
   
   bool success = true;
   bool *psuccess = (data->useDefaultColor ? &success : 0);
   
   AtTextureParams params;
   memcpy(&params, &(data->params), sizeof(AtTextureParams));
   params.scale_s = AiShaderEvalParamFlt(p_sscale);
   params.scale_t = AiShaderEvalParamFlt(p_tscale);
   params.start_channel = AiShaderEvalParamByte(p_start_channel);
   
   // Do not use heavy filtering in secondary rays
   if ((sg->Rt & AI_RAY_DIFFUSE) && (params.filter > AI_TEXTURE_BILINEAR))
   {
      params.filter = AI_TEXTURE_BILINEAR;
   }
   
   AtRGBA rv = AI_RGBA_BLACK;
   
   if (data->evalPath)
   {
      rv = AiTextureAccess(sg, AiShaderEvalParamStr(p_filename), &params, psuccess);
   }
   else
   {
      if (data->handle)
      {
         rv = AiTextureHandleAccess(sg, data->handle, &params, psuccess);
      }
      else
      {
         rv = AiTextureAccess(sg, data->filename, &params, psuccess);
      }
   }
   
   if (data->useDefaultColor && !success)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_default_color);
   }
   else
   {
      AtColor multiply = AiShaderEvalParamRGB(p_multiply);
      AtColor offset = AiShaderEvalParamRGB(p_offset);
      
      sg->out.RGBA.r = offset.r + multiply.r * rv.r;
      sg->out.RGBA.g = offset.g + multiply.g * rv.g;
      sg->out.RGBA.b = offset.b + multiply.b * rv.b;
      sg->out.RGBA.a = rv.a;
   }
}


