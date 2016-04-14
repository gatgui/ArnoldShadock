#include "common.h"
#include <gmath/color.h>

AI_SHADER_NODE_EXPORT_METHODS(BlackbodyMtd);

enum BlackbodyParams
{
   p_temperature = 0,
   p_output_mode,
   p_tm_key,
   p_tm_Lavg,
   p_tm_Lwht,
   p_ca_transform,
   p_ca_max_temperature,
   p_ca_white_point,
   p_exposure,
   p_color_space,
   
   // remap ramp?
};

enum OutputMode
{
   OM_normalize = 0,
   OM_raw,
   OM_tone_mapping,
   OM_physical_intensity,
   OM_chromatic_adaptation
};

const char* OutputModeNames[] = 
{
   "normalize",
   "raw",
   "tone_mapping",
   "physical_intensity",
   "chromatic_adaptation",
   NULL
};

node_parameters
{
   AiParameterFlt("temperature", 3000.0f);
   AiParameterEnum(SSTR::output_mode, OM_physical_intensity, OutputModeNames);
   AiParameterFlt(SSTR::tm_key, 0.18f);
   AiParameterFlt(SSTR::tm_Lavg, 1.0f);
   AiParameterFlt(SSTR::tm_Lwht, 0.0f);
   AiParameterEnum(SSTR::ca_transform, gmath::CAT_VonKries, ChromaticAdaptationTransformNames);
   AiParameterFlt(SSTR::ca_max_temperature, 5000.0f);
   AiParameterRGB(SSTR::ca_white_point, 1.0f, 1.0f, 1.0f);
   AiParameterFlt("exposure", 0.0f);
   AiParameterEnum(SSTR::color_space, CS_Rec709, ColorSpaceNames);
   AiParameterBool(SSTR::fix_nans, false);
   AiParameterRGB(SSTR::nan_color, 0.0f, 0.0f, 0.0f);

   AiMetaDataSetBool(mds, SSTR::output_mode, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::tm_key, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::tm_Lavg, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::tm_Lwht, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::ca_transform, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::ca_max_temperature, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::ca_white_point, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::color_space, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::fix_nans, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::nan_color, SSTR::linkable, false);
}

struct NodeData
{
   OutputMode mode;
   const gmath::ColorSpace *CS;
   gmath::ToneMappingOperator *TM;
   gmath::Matrix3 CAT;
   bool fixNans;
   AtColor nanColor;
};

node_initialize
{
   NodeData *data = new NodeData();
   data->CS = 0;
   data->TM = 0;
   AiNodeSetLocalData(node, (void*)data);
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   if (data->TM)
   {
      delete data->TM;
      data->TM = 0;
   }

   data->mode = (OutputMode) AiNodeGetInt(node, SSTR::output_mode);
   data->CS = ColorSpaces[AiNodeGetInt(node, SSTR::color_space)];
   data->fixNans = AiNodeGetBool(node, SSTR::fix_nans);
   data->nanColor = AiNodeGetRGB(node, SSTR::nan_color);

   if (data->mode == OM_tone_mapping)
   {
      gmath::Params rhp;
      
      rhp.set("key", AiNodeGetFlt(node, SSTR::tm_key));
      rhp.set("Lavg", AiNodeGetFlt(node, SSTR::tm_Lavg));
      rhp.set("Lwht", AiNodeGetFlt(node, SSTR::tm_Lwht));
      
      data->TM = new gmath::ToneMappingOperator(*(data->CS));
      data->TM->setMethod(gmath::ToneMappingOperator::Reinhard, rhp);
      
      if (!data->TM->isValid())
      {
         AiMsgWarning("Invalid tone mapping operator setup in blackbody.");
      }
   }
   else if (data->mode == OM_chromatic_adaptation)
   {
      gmath::ChromaticAdaptationTransform CAT = (gmath::ChromaticAdaptationTransform) AiNodeGetInt(node, SSTR::ca_transform);
      float Kmax = AiNodeGetFlt(node, SSTR::ca_max_temperature);
      AtColor Wp = AiNodeGetRGB(node, SSTR::ca_white_point);
      
      gmath::XYZ from = gmath::Blackbody::GetXYZ(Kmax);
      gmath::XYZ to = data->CS->RGBtoXYZ(gmath::RGB(Wp.r, Wp.g, Wp.b));
      
      data->CAT = data->CS->getXYZtoRGBMatrix() * gmath::ChromaticAdaptationMatrix(from, to, CAT);
   }
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   if (data->TM)
   {
      delete data->TM;
   }
   delete data;
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   float temperature = AiShaderEvalParamFlt(p_temperature);
   float exposure = AiShaderEvalParamFlt(p_exposure);
   float scale = powf(2.0f, exposure);
   
   gmath::RGB out(0.0f, 0.0f, 0.0f);
   
   switch (data->mode)
   {
   case OM_normalize:
      out = gmath::Blackbody::GetRGB(temperature, *(data->CS), true);
      break;
   case OM_raw:
      out = gmath::Blackbody::GetRGB(temperature, *(data->CS), false);
      break;
   case OM_tone_mapping:
      out = data->CS->XYZtoRGB((*data->TM)(gmath::Blackbody::GetXYZ(temperature)));
      break;
   case OM_physical_intensity:
      // Use Stefan-Boltzman law to compute light intensity (W.m^-2) from temperature
      //   intensity = 5.670373e-8 * pow(temperature, 4)
      out = gmath::Blackbody::GetRGB(temperature, *(data->CS), true);
      scale *= 5.670373e-8 * powf(temperature, 4.0f);
      break;
   case OM_chromatic_adaptation:
      {
         gmath::XYZ tmp = gmath::Blackbody::GetXYZ(temperature);
         gmath::Vector3 v = data->CAT * gmath::Vector3(tmp.x, tmp.y, tmp.z);
         out.r = v.x;
         out.g = v.y;
         out.b = v.z;
      }
      break;
   default:
      break;
   }

   sg->out.RGB.r = scale * out.r;
   sg->out.RGB.g = scale * out.g;
   sg->out.RGB.b = scale * out.b;
   
   if (data->fixNans)
   {
      if (AiColorCorrupted(sg->out.RGB))
      {
         sg->out.RGB = data->nanColor;
      }
   }
}
