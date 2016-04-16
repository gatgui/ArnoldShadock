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
   AiParameterEnum("filter", Filter_smart_bicubic, FilterNames);
   AiParameterEnum("mipmap_mode", Mip_default, MipNames);
   AiParameterInt("mipmap_bias", 0);
   AiParameterBool("single_channel", false);
   AiParameterFlt("fill", 1.0f);
   AiParameterByte("start_channel", 0);
   AiParameterEnum("swrap", Wrap_periodic, WrapNames);
   AiParameterEnum("twrap", Wrap_periodic, WrapNames);
   AiParameterFlt("sscale", 1.0f);
   AiParameterFlt("tscale", 1.0f);
   AiParameterBool("sflip", false);
   AiParameterBool("tflip", false);
   AiParameterBool("swap_st", false);
   AiParameterFlt("swidth", 1.0f);
   AiParameterFlt("twidth", 1.0f);
   AiParameterFlt("sblur", 0.0f);
   AiParameterFlt("tblur", 0.0f);
   AiParameterRGB("multiply", 1.0f, 1.0f, 1.0f);
   AiParameterRGB("offset", 0.0f, 0.0f, 0.0f);
   AiParameterBool("cache_texture_handles", true);
   AiParameterBool("use_default_color", false);
   AiParameterRGBA(SSTR::default_color, 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiMetaDataSetBool(mds, "filter", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "mipmap_mode", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "mipmap_bias", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "single_channel", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "fill", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "swrap", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "twrap", SSTR::linkable, false);
   //AiMetaDataSetBool(mds, "sscale", SSTR::linkable, false);
   //AiMetaDataSetBool(mds, "tscale", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "sflip", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "tflip", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "swidth", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "twidth", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "sblur", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "tblur", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "swap_st", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "cache_texture_handles", SSTR::linkable, false);
   AiMetaDataSetBool(mds, "use_default_color", SSTR::linkable, false);
   
   AiMetaDataSetBool(mds, SSTR::filename, SSTR::filepath, true);
   AiMetaDataSetStr(mds, SSTR::filename, "houdini.type", "file:image");
}

struct NodeData
{
   bool evalPath;
   const char *filename;
   AtTextureHandle *handle;
   AtTextureParams params;
   bool useDefaultColor;
};


node_initialize
{
   NodeData *data = new NodeData();
   AddMemUsage<NodeData>();
   
   // initialize for first node_update call
   data->handle = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->evalPath = AiNodeIsLinked(node, SSTR::filename);
   data->filename = 0;
   if (data->handle)
   {
      AiTextureHandleDestroy(data->handle);
   }
   data->handle = 0;
   data->useDefaultColor = AiNodeGetBool(node, "use_default_color");
   AiTextureParamsSetDefaults(&(data->params));
   data->params.filter = AiNodeGetInt(node, "filter");
   data->params.mipmap_mode = AiNodeGetInt(node, "mipmap_mode");
   data->params.mipmap_bias = AiNodeGetInt(node, "mipmap_bias");
   data->params.single_channel = AiNodeGetBool(node, "single_channel");
   data->params.fill = AiNodeGetFlt(node, "fill");
   data->params.flip_s = AiNodeGetBool(node, "sflip");
   data->params.flip_t = AiNodeGetBool(node, "tflip");
   data->params.swap_st = AiNodeGetBool(node, "swap_st");
   data->params.wrap_s = AiNodeGetInt(node, "swrap");
   data->params.wrap_t = AiNodeGetInt(node, "twrap");
   data->params.width_s = AiNodeGetFlt(node, "swidth");
   data->params.width_t = AiNodeGetFlt(node, "twidth");
   data->params.blur_s = AiNodeGetFlt(node, "sblur");
   data->params.blur_t = AiNodeGetFlt(node, "tblur");
   
   if (!data->evalPath)
   {
      data->filename = AiNodeGetStr(node, SSTR::filename);
      
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
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   if (data->handle)
   {
      AiTextureHandleDestroy(data->handle);
   }
   
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
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


