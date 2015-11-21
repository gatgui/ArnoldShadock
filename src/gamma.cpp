#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GammaMtd);

enum GammaParams
{
   p_input = 0,
   p_mode,
   p_transform,
   p_logc_el
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
   gmath::Gamma::NonLinearTransform nlt;
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
      data->nlt = gmath::Gamma::NLT_Gamma22;
      break;
   case GT_Gamma24:
      data->nlt = gmath::Gamma::NLT_Gamma24;
      break;
   case GT_sRGB:
      data->nlt = gmath::Gamma::NLT_sRGB;
      break;
   case GT_Rec709:
      data->nlt = gmath::Gamma::NLT_Rec709;
      break;
   case GT_LogC:
      data->nlt = (gmath::Gamma::NonLinearTransform) (int(gmath::Gamma::NLT_LogCv3) + (AiNodeGetInt(node, "logc_exposure_level") - 7));
      break;
   case GT_Cineon:
      data->nlt = gmath::Gamma::NLT_Cineon;
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
         out = gmath::Gamma::Linearize(in, data->nlt);
      }
      else
      {
         out = gmath::Gamma::Unlinearize(in, data->nlt);
      }

      sg->out.RGB.r = out.r;
      sg->out.RGB.g = out.g;
      sg->out.RGB.b = out.b;
   }
}
