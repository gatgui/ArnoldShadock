#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GammaMtd);

enum GammaParams
{
   p_input = 0,
   p_mode,
   p_transform,
   p_logc_exposure
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::mode, GM_Expand, GammaModeNames);
   AiParameterEnum(SSTR::transform, GF_sRGB, GammaFunctionNames);
   AiParameterEnum(SSTR::logc_exposure, EL_800, LogCExposureLevelNames);
}

struct NodeData
{
   bool valid;
   bool expand;
   gmath::Gamma::Function gf;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   data->valid = true;
   data->expand = (AiNodeGetInt(node, SSTR::mode) == GM_Expand);

   switch (AiNodeGetInt(node, SSTR::transform))
   {
   case GF_Power22:
      data->gf = gmath::Gamma::Power22;
      break;
   case GF_Power24:
      data->gf = gmath::Gamma::Power24;
      break;
   case GF_sRGB:
      data->gf = gmath::Gamma::sRGB;
      break;
   case GF_Rec709:
      data->gf = gmath::Gamma::Rec709;
      break;
   case GF_LogC:
      data->gf = (gmath::Gamma::Function) (int(gmath::Gamma::LogCv3) + (AiNodeGetInt(node, SSTR::logc_exposure) - 7));
      break;
   case GF_Cineon:
      data->gf = gmath::Gamma::Cineon;
      break;
   case GF_Rec2020:
      data->gf = gmath::Gamma::Rec2020;
      break;
   default:
      data->valid = false;
      break;
   }
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
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
         out = gmath::Gamma::Linearize(in, data->gf);
      }
      else
      {
         out = gmath::Gamma::Unlinearize(in, data->gf);
      }

      sg->out.RGB.r = out.r;
      sg->out.RGB.g = out.g;
      sg->out.RGB.b = out.b;
   }
}
