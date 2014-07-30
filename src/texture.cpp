#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TextureMtd);

enum TextureParams
{
   p_filename = 0,
   p_filter,
   #ifdef ARNOLD_4_1_AND_ABOVE
   p_mipmap_mode,
   #endif
   p_mipmap_bias,
   p_single_channel,
   #ifdef ARNOLD_4_1_AND_ABOVE
   p_fill,
   #  ifdef HAS_START_CHANNEL
   p_start_channel,
   #  endif
   #endif
   p_swrap,
   p_twrap,
   p_sscale,
   p_tscale,
   p_sflip,
   p_tflip,
   p_swap_st,
   p_swidth,
   p_twidth,
   #ifdef ARNOLD_4_1_AND_ABOVE
   p_sblur,
   p_tblur,
   #endif
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

struct TextureData
{
   bool path_is_linked;
   const char *filename;
   AtTextureHandle *handle;
   AtTextureParams params;
   bool use_default_color;
};

node_parameters
{
   AiParameterStr("filename", "");
   AiParameterEnum("filter", Filter_smart_bicubic, FilterNames);
   #ifdef ARNOLD_4_1_AND_ABOVE
   AiParameterEnum("mipmap_mode", Mip_default, MipNames);
   #endif
   AiParameterInt("mipmap_bias", 0);
   AiParameterBool("single_channel", false);
   #ifdef ARNOLD_4_1_AND_ABOVE
   AiParameterFlt("fill", 1.0f);
   #  ifdef HAS_START_CHANNEL
   AiParameterByte("start_channel", 0);
   #  endif
   #endif
   AiParameterEnum("swrap", Wrap_periodic, WrapNames);
   AiParameterEnum("twrap", Wrap_periodic, WrapNames);
   AiParameterFlt("sscale", 1.0f);
   AiParameterFlt("tscale", 1.0f);
   AiParameterBool("sflip", false);
   AiParameterBool("tflip", false);
   AiParameterBool("swap_st", false);
   AiParameterFlt("swidth", 1.0f);
   AiParameterFlt("twidth", 1.0f);
   #ifdef ARNOLD_4_1_AND_ABOVE
   AiParameterFlt("sblur", 0.0f);
   AiParameterFlt("tblur", 0.0f);
   #endif
   AiParameterRGB("multiply", 1.0f, 1.0f, 1.0f);
   AiParameterRGB("offset", 0.0f, 0.0f, 0.0f);
   AiParameterBool("cache_texture_handles", true);
   AiParameterBool("use_default_color", false);
   AiParameterRGBA("default_color", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiMetaDataSetBool(mds, "filter", "linkable", false);
   #ifdef ARNOLD_4_1_AND_ABOVE
   AiMetaDataSetBool(mds, "mipmap_mode", "linkable", false);
   #endif
   AiMetaDataSetBool(mds, "mipmap_bias", "linkable", false);
   AiMetaDataSetBool(mds, "single_channel", "linkable", false);
   #ifdef ARNOLD_4_1_AND_ABOVE
   AiMetaDataSetBool(mds, "fill", "linkable", false);
   #endif
   AiMetaDataSetBool(mds, "swrap", "linkable", false);
   AiMetaDataSetBool(mds, "twrap", "linkable", false);
   //AiMetaDataSetBool(mds, "sscale", "linkable", false);
   //AiMetaDataSetBool(mds, "tscale", "linkable", false);
   AiMetaDataSetBool(mds, "sflip", "linkable", false);
   AiMetaDataSetBool(mds, "tflip", "linkable", false);
   AiMetaDataSetBool(mds, "swidth", "linkable", false);
   AiMetaDataSetBool(mds, "twidth", "linkable", false);
   #ifdef ARNOLD_4_1_AND_ABOVE
   AiMetaDataSetBool(mds, "sblur", "linkable", false);
   AiMetaDataSetBool(mds, "tblur", "linkable", false);
   #endif
   AiMetaDataSetBool(mds, "swap_st", "linkable", false);
   AiMetaDataSetBool(mds, "cache_texture_handles", "linkable", false);
   AiMetaDataSetBool(mds, "use_default_color", "linkable", false);
   
   AiMetaDataSetBool(mds, "filename", "filepath", true);
   AiMetaDataSetStr(mds, "filename", "houdini.type", "file:image");
}

node_initialize
{
   TextureData *data = (TextureData*) AiMalloc(sizeof(TextureData));
   
   // initialize for first node_update call
   data->handle = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   TextureData *data = (TextureData*) AiNodeGetLocalData(node);
   
   data->path_is_linked = AiNodeIsLinked(node, "filename");
   data->filename = 0;
   if (data->handle)
   {
      AiTextureHandleDestroy(data->handle);
   }
   data->handle = 0;
   data->use_default_color = AiNodeGetBool(node, "use_default_color");
   AiTextureParamsSetDefaults(&(data->params));
   data->params.filter = AiNodeGetInt(node, "filter");
   #ifdef ARNOLD_4_1_AND_ABOVE
   data->params.mipmap_mode = AiNodeGetInt(node, "mipmap_mode");
   #endif
   data->params.mipmap_bias = AiNodeGetInt(node, "mipmap_bias");
   data->params.single_channel = AiNodeGetBool(node, "single_channel");
   #ifdef ARNOLD_4_1_AND_ABOVE
   data->params.fill = AiNodeGetFlt(node, "fill");
   #endif
   data->params.flip_s = AiNodeGetBool(node, "sflip");
   data->params.flip_t = AiNodeGetBool(node, "tflip");
   data->params.swap_st = AiNodeGetBool(node, "swap_st");
   data->params.wrap_s = AiNodeGetInt(node, "swrap");
   data->params.wrap_t = AiNodeGetInt(node, "twrap");
   data->params.width_s = AiNodeGetFlt(node, "swidth");
   data->params.width_t = AiNodeGetFlt(node, "twidth");
   #ifdef ARNOLD_4_1_AND_ABOVE
   data->params.blur_s = AiNodeGetFlt(node, "sblur");
   data->params.blur_t = AiNodeGetFlt(node, "tblur");
   #endif
   
   if (!data->path_is_linked)
   {
      data->filename = AiNodeGetStr(node, "filename");
      
      if (AiNodeGetBool(node, "cache_texture_handles"))
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
   
   AiFree(data);
}

shader_evaluate
{
   TextureData *data = (TextureData*) AiNodeGetLocalData(node);
   
   bool success = true;
   bool *psuccess = (data->use_default_color ? &success : 0);
   
   AtTextureParams params;
   memcpy(&params, &(data->params), sizeof(AtTextureParams));
   params.scale_s = AiShaderEvalParamFlt(p_sscale);
   params.scale_t = AiShaderEvalParamFlt(p_tscale);
   #ifdef HAS_START_CHANNEL
   params.start_channel = AiShaderEvalParamByte(p_start_channel);
   #endif
   
   // Do not use heavy filtering in secondary rays
   if ((sg->Rt & AI_RAY_DIFFUSE) && (params.filter > AI_TEXTURE_BILINEAR))
   {
      params.filter = AI_TEXTURE_BILINEAR;
   }
   
   AtRGBA rv = AI_RGBA_BLACK;
   
   if (data->path_is_linked)
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
   
   if (data->use_default_color && !success)
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


