#include <ai.h>

AI_SHADER_NODE_EXPORT_METHODS(VolumeMtd);

enum VolumeParams
{
   p_scattering = 0,
   p_scattering_color,
   p_anisotropy,
   p_attenuation_mode,
   p_attenuation_use_scattering,
   p_attenuation,
   p_attenuation_color,
   p_preserve_energy,
   p_emission,
   p_emission_color
};

enum AttenuationMode
{
   AM_absorption = 0,
   AM_extinction,
};

static const char* AttenuationModeNames[] =
{
   "absorption",
   "extinction",
   NULL
};

struct ShaderData
{
   float scattering;
   AtColor scattering_color;
   float anisotropy;
   
   int attenuation_mode;
   bool attenuation_use_scattering;
   float attenuation;
   AtColor attenuation_color;
   bool preserve_energy;
   
   float emission;
   AtColor emission_color;
   
   bool scattering_linked;
   bool scattering_color_linked;
   bool anisotropy_linked;
   bool attenuation_linked;
   bool attenuation_color_linked;
   bool emission_linked;
   bool emission_color_linked;
};

node_parameters
{
   AiParameterFlt("scattering", 1.0f);
   AiParameterRGB("scattering_color", 1.0f, 1.0f, 1.0f);
   AiParameterFlt("anisotropy", 0.0f);
   
   AiParameterEnum("attenuation_mode", AM_absorption, AttenuationModeNames);
   AiParameterBool("attenuation_use_scattering", false);
   AiParameterFlt("attenuation", 1.0f);
   AiParameterRGB("attenuation_color", 1.0f, 1.0f, 1.0f);
   AiParameterBool("preserve_energy", true);
   
   AiParameterFlt("emission", 1.0f);
   AiParameterRGB("emission_color", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "attenuation_mode", "linkable", false);
   AiMetaDataSetBool(mds, "attenuation_use_scattering", "linkable", false);
   AiMetaDataSetBool(mds, "preserve_energy", "linkable", false);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(ShaderData)));
}

node_update
{
   ShaderData* data = (ShaderData*) AiNodeGetLocalData(node);
   
   data->scattering_linked = AiNodeIsLinked(node, "scattering");
   if (!data->scattering_linked)
   {
      data->scattering = AiNodeGetFlt(node, "scattering");
   }
   data->scattering_color_linked = AiNodeIsLinked(node, "scattering_color");
   if (!data->scattering_color_linked)
   {
      data->scattering_color = AiNodeGetRGB(node, "scattering_color");
   }
   data->anisotropy_linked = AiNodeIsLinked(node, "anisotropy");
   if (!data->anisotropy_linked)
   {
      data->anisotropy = AiNodeGetFlt(node, "anisotropy");
   }
   
   data->attenuation_mode = AiNodeGetInt(node, "attenuation_mode");
   data->attenuation_use_scattering = AiNodeGetBool(node, "attenuation_use_scattering");
   data->attenuation_linked = AiNodeIsLinked(node, "attenuation");
   if (!data->attenuation_linked)
   {
      data->attenuation = AiNodeGetFlt(node, "attenuation");
   }
   data->attenuation_color_linked = AiNodeIsLinked(node, "attenuation_color");
   if (!data->attenuation_color_linked)
   {
      data->attenuation_color = AiNodeGetRGB(node, "attenuation_color");
   }
   data->preserve_energy = AiNodeGetBool(node, "preserve_energy");
   
   data->emission_linked = AiNodeIsLinked(node, "emission");
   if (!data->emission_linked)
   {
      data->emission = AiNodeGetFlt(node, "emission");
   }
   data->emission_color_linked = AiNodeIsLinked(node, "emission_color");
   if (!data->emission_color_linked)
   {
      data->emission_color = AiNodeGetRGB(node, "emission_color");
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   ShaderData* data = (ShaderData*) AiNodeGetLocalData(node);
   
   AtColor scattering = (data->scattering_color_linked ? AiShaderEvalParamRGB(p_scattering_color) : data->scattering_color);
   scattering *= (data->scattering_linked ? AiShaderEvalParamFlt(p_scattering) : data->scattering);
   
   AtColor attenuation;
   if (data->attenuation_use_scattering)
   {
      attenuation = scattering;
   }
   else
   {
      attenuation = (data->attenuation_color_linked ? AiShaderEvalParamRGB(p_attenuation_color) : data->attenuation_color);
      attenuation *= (data->attenuation_linked ? AiShaderEvalParamFlt(p_attenuation) : data->attenuation);
   }
   
   AtColor emission = (data->emission_color_linked ? AiShaderEvalParamRGB(p_emission_color) : data->emission_color);
   emission *= (data->emission_linked ? AiShaderEvalParamFlt(p_emission) : data->emission);
   
   float anisotropy = (data->anisotropy_linked ? AiShaderEvalParamFlt(p_anisotropy) : data->anisotropy);
   
   AiColorClipToZero(scattering);
   AiColorClipToZero(attenuation);
   AiColorClipToZero(emission);
   
   AiShaderGlobalsSetVolumeScattering(sg, scattering, anisotropy);
   
   switch (data->attenuation_mode)
   {
   case AM_extinction:
      if (!data->attenuation_use_scattering && data->preserve_energy)
      {
         // For energy preservation we must have scattering <= attenuation
         // Adjust attenuation to match scattering
         if (scattering.r > attenuation.r)
         {
            attenuation.r = scattering.r;
         }
         if (scattering.g > attenuation.g)
         {
            attenuation.g = scattering.g;
         }
         if (scattering.b > attenuation.b)
         {
            attenuation.b = scattering.b;
         }
      }
      AiShaderGlobalsSetVolumeAttenuation(sg, attenuation);
      break;
   
   case AM_absorption:
   default:
      AiShaderGlobalsSetVolumeAbsorption(sg, attenuation);
      break;
   }
   
   if (!AiColorIsZero(emission))
   {
      AiShaderGlobalsSetVolumeEmission(sg, emission);
   }
}


