#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ImageResolutionMtd);

enum ImageResolutionParams
{
   p_filename = 0
};

struct ImageResolutionData
{
   bool is_linked;
   bool valid;
   unsigned int w;
   unsigned int h;
};

node_parameters
{
   AiParameterStr("filename", "");
   
   AiMetaDataSetBool(mds, "filename", "filepath", true);
   
   AiMetaDataSetStr(mds, "filename", "houdini.type", "file:image");
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(ImageResolutionData)));
}

node_update
{
   ImageResolutionData *data = (ImageResolutionData*) AiNodeGetLocalData(node);
   
   data->is_linked = AiNodeIsLinked(node, "filename");
   
   if (!data->is_linked)
   {
      const char *filename = AiNodeGetStr(node, "filename");
      data->valid = (filename && filename[0] != '\0' && AiTextureGetResolution(filename, &(data->w), &(data->h)));
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   ImageResolutionData *data = (ImageResolutionData*) AiNodeGetLocalData(node);
   
   if (data->is_linked)
   {
      unsigned int w, h;
      const char *filename = AiShaderEvalParamStr(p_filename);
      
      if (filename && filename[0] != '\0' && AiTextureGetResolution(filename, &w, &h))
      {
         sg->out.VEC.x = float(w);
         sg->out.VEC.y = float(h);
      }
      else
      {
         sg->out.VEC.x = 0.0f;
         sg->out.VEC.y = 0.0f;
      }
   }
   else
   {
      sg->out.VEC.x = (data->valid ? float(data->w) : 0.0f);
      sg->out.VEC.y = (data->valid ? float(data->h) : 0.0f);
   }
   
   sg->out.VEC.z = (sg->out.VEC.y >= AI_EPSILON ? (sg->out.VEC.x / sg->out.VEC.y) : 0.0f);
}

