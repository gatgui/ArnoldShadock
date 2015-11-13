#include "common.h"
#include <gmath/color.h>

AI_SHADER_NODE_EXPORT_METHODS(GammaMtd);

enum GammaParams
{
   p_input = 0,
   p_mode,
   p_transform,
   p_logc_el
};

enum GammaMode
{
   GM_Expand = 0,
   GM_Compress
};

static const char* GammaModeNames[] = {
   "expand",
   "compress",
   NULL
};

enum GammaTransform
{
   GT_Gamma22 = 0,
   GT_Gamma24,
   GT_sRGB,
   GT_Rec709,
   GT_LogC,
   GT_Cineon
};

static const char* GammaTransformNames[] = {
   "2.2",
   "2.4",
   "sRGB",
   "Rec. 709",
   "LogC",
   "Cineon",
   NULL
};

enum GammaExposureLevel
{
   GL_160 = 0,
   GL_200,
   GL_250,
   GL_320,
   GL_400,
   GL_500,
   GL_640,
   GL_800,
   GL_1000,
   GL_1280,
   GL_1600
};

static const char* GammaExposureLevelNames[] = {
   "160",
   "200",
   "250",
   "320",
   "400",
   "500",
   "640",
   "800",
   "1000",
   "1280",
   "1600",
   NULL
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("mode", GM_Expand, GammaModeNames);
   AiParameterEnum("transform", GT_sRGB, GammaTransformNames);
   AiParameterEnum("logc_exposure_level", GL_800, GammaExposureLevelNames);

   AiMetaDataSetBool(mds, "mode", "linkable", false);
   AiMetaDataSetBool(mds, "transform", "linkable", false);
   AiMetaDataSetBool(mds, "logc_exposure_level", "linkable", false);
}

struct NodeData
{
   bool valid;
   bool expand;
   gmath::NonLinearTransform nlt;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   data->valid = true;
   data->expand = (AiNodeGetInt(node, "mode") == GM_Expand);

   switch (AiNodeGetInt(node, "transform"))
   {
   case GT_Gamma22:
      data->nlt = gmath::NLT_Gamma22;
      break;
   case GT_Gamma24:
      data->nlt = gmath::NLT_Gamma24;
      break;
   case GT_sRGB:
      data->nlt = gmath::NLT_sRGB;
      break;
   case GT_Rec709:
      data->nlt = gmath::NLT_Rec709;
      break;
   case GT_LogC:
      data->nlt = (gmath::NonLinearTransform) (int(gmath::NLT_LogCv3) + (AiNodeGetInt(node, "logc_exposure_level") - 7));
      break;
   case GT_Cineon:
      data->nlt = gmath::NLT_Cineon;
      break;
   default:
      data->valid = false;
      break;
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   AtRGB input = AiShaderEvalParamRGB(p_input);

   if (!data->valid)
   {
      sg->out.RGB = input;
   }
   else
   {
      gmath::RGB in(input.r, input.g, input.b);
      gmath::RGB out;

      if (data->expand)
      {
         out = gmath::Linearize(in, data->nlt);
      }
      else
      {
         out = gmath::Unlinearize(in, data->nlt);
      }

      sg->out.RGB.r = out.r;
      sg->out.RGB.g = out.g;
      sg->out.RGB.b = out.b;
   }
}
