#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(VolumeMtd);

enum VolumeParams
{
   p_scattering_source = 0,
   p_scattering_field,
   p_scattering,
   p_scattering_weight,
   p_scattering_color,
   p_anisotropy,
   p_attenuation_source,
   p_attenuation_field,
   p_attenuation,
   p_attenuation_weight,
   p_attenuation_color,
   p_attenuation_mode,
   p_preserve_energy,
   p_emission_source,
   p_emission_field,
   p_emission,
   p_emission_weight,
   p_emission_color,
   p_interpolation,
   p_P,
   p_P_space,
   p_P_is_offset,
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

enum Source
{
   S_parameter = 0,
   S_float_field,
   S_color_field,
   S_scattering
};

static const char* SourceNames[] =
{
   "parameter",
   "float_field",
   "color_field",
   NULL
};

static const char* SourceNamesEx[] =
{
   "parameter",
   "float_field",
   "color_field",
   "scattering",
   NULL
};

enum Space
{
   S_world = 0,
   S_object
};

static const char* SpaceNames[] =
{
   "world",
   "object",
   NULL
};

node_parameters
{
   AiParameterEnum(SSTR::scattering_source, S_parameter, SourceNames);
   AiParameterStr(SSTR::scattering_field, "");
   AiParameterRGB(SSTR::scattering, 1.0f, 1.0f, 1.0f);
   AiParameterFlt(SSTR::scattering_weight, 1.0f);
   AiParameterRGB(SSTR::scattering_color, 1.0f, 1.0f, 1.0f);
   AiParameterFlt(SSTR::anisotropy, 0.0f);
   
   AiParameterEnum(SSTR::attenuation_source, S_parameter, SourceNamesEx);
   AiParameterStr(SSTR::attenuation_field, "");
   AiParameterRGB(SSTR::attenuation, 1.0f, 1.0f, 1.0f);
   AiParameterFlt(SSTR::attenuation_weight, 1.0f);
   AiParameterRGB(SSTR::attenuation_color, 1.0f, 1.0f, 1.0f);
   AiParameterEnum(SSTR::attenuation_mode, AM_absorption, AttenuationModeNames);
   AiParameterBool(SSTR::preserve_energy, true);
   
   AiParameterEnum(SSTR::emission_source, S_parameter, SourceNames);
   AiParameterStr(SSTR::emission_field, "");
   AiParameterRGB(SSTR::emission, 0.0f, 0.0f, 0.0f);
   AiParameterFlt(SSTR::emission_weight, 1.0f);
   AiParameterRGB(SSTR::emission_color, 1.0f, 1.0f, 1.0f);

   AiParameterEnum(SSTR::interpolation, VI_linear, VolumeInterpNames);
   AiParameterPnt(SSTR::P, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::P_space, S_world, SpaceNames);
   AiParameterBool(SSTR::P_is_offset, true);
}

struct VolumeData
{
   Source scatteringSource;
   AtString scatteringField;
   bool evalScattering;
   AtColor scattering;
   bool evalScatteringWeight;
   float scatteringWeight;
   bool evalScatteringColor;
   AtColor scatteringColor;
   bool evalAnisotropy;
   float anisotropy;
   
   Source attenuationSource;
   AtString attenuationField;
   bool evalAttenuation;
   AtColor attenuation;
   bool evalAttenuationWeight;
   float attenuationWeight;
   bool evalAttenuationColor;
   AtColor attenuationColor;
   int attenuationMode;
   bool preserveEnergy;
   
   Source emissionSource;
   AtString emissionField;
   bool evalEmission;
   AtColor emission;
   bool evalEmissionWeight;
   float emissionWeight;
   bool evalEmissionColor;
   AtColor emissionColor;
   
   int interpolation;
   bool evalP;
   AtPoint P;
   Space space;
   bool isOffset;
};

node_initialize
{
   AiNodeSetLocalData(node, new VolumeData());
   AddMemUsage<VolumeData>();
}

node_update
{
   VolumeData* data = (VolumeData*) AiNodeGetLocalData(node);
   
   // Scattering
   data->scatteringSource = (Source) AiNodeGetInt(node, SSTR::scattering_source);
   if (data->scatteringSource != S_parameter)
   {
      data->scatteringField = AiNodeGetStr(node, SSTR::scattering_field);
   }
   data->evalScattering = (data->scatteringSource != S_parameter || AiNodeIsLinked(node, SSTR::scattering));
   if (!data->evalScattering)
   {
      data->scattering = AiNodeGetRGB(node, SSTR::scattering);
   }
   data->evalScatteringWeight = AiNodeIsLinked(node, SSTR::scattering_weight);
   if (!data->evalScatteringWeight)
   {
      data->scatteringWeight = AiNodeGetFlt(node, SSTR::scattering_weight);
   }
   data->evalScatteringColor = AiNodeIsLinked(node, SSTR::scattering_color);
   if (!data->evalScatteringColor)
   {
      data->scatteringColor = AiNodeGetRGB(node, SSTR::scattering_color);
   }
   data->evalAnisotropy = AiNodeIsLinked(node, SSTR::anisotropy);
   if (!data->evalAnisotropy)
   {
      data->anisotropy = AiNodeGetFlt(node, SSTR::anisotropy);
   }
   
   // Attenuation
   data->attenuationSource = (Source) AiNodeGetInt(node, SSTR::attenuation_source);
   if (data->attenuationSource != S_parameter)
   {
      data->attenuationField = AiNodeGetStr(node, SSTR::attenuation_field);
   }
   data->evalAttenuation = (data->attenuationSource != S_parameter || AiNodeIsLinked(node, SSTR::attenuation));
   if (!data->evalAttenuation)
   {
      data->attenuation = AiNodeGetRGB(node, SSTR::attenuation);
   }
   data->evalAttenuationWeight = AiNodeIsLinked(node, SSTR::attenuation_weight);
   if (!data->evalAttenuationWeight)
   {
      data->attenuationWeight = AiNodeGetFlt(node, SSTR::attenuation_weight);
   }
   data->evalAttenuationColor = AiNodeIsLinked(node, SSTR::attenuation_color);
   if (!data->evalAttenuationColor)
   {
      data->attenuationColor = AiNodeGetRGB(node, SSTR::attenuation_color);
   }
   data->attenuationMode = AiNodeGetInt(node, SSTR::attenuation_mode);
   data->preserveEnergy = AiNodeGetBool(node, SSTR::preserve_energy);

   // Emission
   data->emissionSource = (Source) AiNodeGetInt(node, SSTR::emission_source);
   if (data->emissionSource != S_parameter)
   {
      data->emissionField = AiNodeGetStr(node, SSTR::emission_field);
   }
   data->evalEmission = (data->emissionSource != S_parameter || AiNodeIsLinked(node, SSTR::emission));
   if (!data->evalEmission)
   {
      data->emission = AiNodeGetRGB(node, SSTR::emission);
   }
   data->evalEmissionWeight = AiNodeIsLinked(node, SSTR::emission_weight);
   if (!data->evalEmissionWeight)
   {
      data->emissionWeight = AiNodeGetFlt(node, SSTR::emission_weight);
   }
   data->evalEmissionColor = AiNodeIsLinked(node, SSTR::emission_color);
   if (!data->evalEmissionColor)
   {
      data->emissionColor = AiNodeGetRGB(node, SSTR::emission_color);
   }

   data->interpolation = AiNodeGetInt(node, SSTR::interpolation);
   data->evalP = AiNodeIsLinked(node, SSTR::P);
   if (!data->evalP)
   {
      data->P = AiNodeGetPnt(node, SSTR::P);
   }
   data->space = (Space) AiNodeGetInt(node, SSTR::P_space);
   data->isOffset = AiNodeGetBool(node, SSTR::P_is_offset);
}

node_finish
{
   VolumeData* data = (VolumeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<VolumeData>();
}

shader_evaluate
{
   VolumeData* data = (VolumeData*) AiNodeGetLocalData(node);
   
   // Override/Offset P
   AtPoint oldP = sg->P;
   AtPoint oldPo = sg->Po;
   
   AtPoint P = (data->evalP ? AiShaderEvalParamPnt(p_P) : data->P);

   if (!data->isOffset || !AiV3IsZero(P))
   {
      switch (data->space)
      {
      case S_object:
         sg->Po = (data->isOffset ? (sg->Po + P) : P);
         AiM4PointByMatrixMult(&(sg->P), sg->M, &(sg->Po));
         break;
      case S_world:
      default:
         sg->P = (data->isOffset ? (sg->P + P) : P);
         AiM4PointByMatrixMult(&(sg->Po), sg->Minv, &(sg->P));
      }
   }

   // Temporaries
   float fvalue;
   AtColor cvalue;

   // Attenuation
   AtColor attenuation = AI_RGB_BLACK;
   AtColor scattering = AI_RGB_BLACK;
   bool scatteringRead = false;

   cvalue = (data->evalAttenuationColor ? AiShaderEvalParamRGB(p_attenuation_color) : data->attenuationColor);
   cvalue *= (data->evalAttenuationWeight ? AiShaderEvalParamFlt(p_attenuation_weight) : data->attenuationWeight);
   
   // Avoid possible field evaluation if weight * color is zero
   if (!AiColorIsZero(cvalue))
   {
      switch (data->attenuationSource)
      {
      case S_scattering:
         switch (data->scatteringSource)
         {
         case S_float_field:
            fvalue = 0.0f;
            if (AiVolumeSampleFlt(data->scatteringField, data->interpolation, &fvalue))
            {
               scattering.r = fvalue;
               scattering.g = fvalue;
               scattering.b = fvalue;
            }
            break;
         case S_color_field:
            AiVolumeSampleRGB(data->scatteringField, data->interpolation, &scattering);
            break;
         case S_parameter:
         default:
            scattering = (data->evalScattering ? AiShaderEvalParamRGB(p_scattering) : data->scattering);
         }
         scatteringRead = true;
         attenuation = scattering;
         break;
      case S_float_field:
         fvalue = 0.0f;
         if (AiVolumeSampleFlt(data->attenuationField, data->interpolation, &fvalue))
         {
            attenuation.r = fvalue;
            attenuation.g = fvalue;
            attenuation.b = fvalue;
         }
         break;
      case S_color_field:
         AiVolumeSampleRGB(data->attenuationField, data->interpolation, &attenuation);
         break;
      case S_parameter:
      default:
         attenuation = (data->evalAttenuation ? AiShaderEvalParamRGB(p_attenuation) : data->attenuation);
      }

      attenuation *= cvalue;
   }

   AiColorClipToZero(attenuation);
   
   switch (data->attenuationMode)
   {
   case AM_extinction:
      AiShaderGlobalsSetVolumeAttenuation(sg, attenuation);
      break;
   case AM_absorption:
   default:
      AiShaderGlobalsSetVolumeAbsorption(sg, attenuation);
   }
   
   // Scattering
   // 
   // When attenuation mode is set to 'absorption', we need to set the scattering coefficient as it
   //   influences the total attenuation (see arnold volume API documentation)
   // 
   // 'absorption' mode:
   //    AiShaderGlobalsSetVolumeAbsorption(sg, absorption_coefficient)
   //    => att = expf(-(scattering_coefficient + absorption_coefficient) * distance_travelled)
   // 
   // 'extinction' mode:
   //    AiShaderGlobalsSetVolumeAttenuation(sg, attenuation_coefficient)
   //    => att = expf(-attenuation_coefficient * distance_travelled)
   if (!(sg->Rt & AI_RAY_SHADOW) || data->attenuationMode == AM_absorption)
   {
      cvalue = (data->evalScatteringColor ? AiShaderEvalParamRGB(p_scattering_color) : data->scatteringColor);
      cvalue *= (data->evalScatteringWeight ? AiShaderEvalParamFlt(p_scattering_weight) : data->scatteringWeight);

      // Avoid possible field evaluation if weight * color is zero
      if (!AiColorIsZero(cvalue))
      {
         // Scattering may have been read when setting up attenuation
         if (!scatteringRead)
         {
            switch (data->scatteringSource)
            {
            case S_float_field:
               fvalue = 0.0f;
               if (AiVolumeSampleFlt(data->scatteringField, data->interpolation, &fvalue))
               {
                  scattering.r = fvalue;
                  scattering.g = fvalue;
                  scattering.b = fvalue;
               }
               break;
            case S_color_field:
               AiVolumeSampleRGB(data->scatteringField, data->interpolation, &scattering);
               break;
            case S_parameter:
            default:
               scattering = (data->evalScattering ? AiShaderEvalParamRGB(p_scattering) : data->scattering);
            }
         }

         cvalue *= scattering;
      }

      AiColorClipToZero(cvalue);

      if (data->attenuationMode == AM_extinction && data->preserveEnergy)
      {
         // For energy preservation we must have scattering <= attenuation
         if (cvalue.r > attenuation.r) cvalue.r = attenuation.r;
         if (cvalue.g > attenuation.g) cvalue.g = attenuation.g;
         if (cvalue.b > attenuation.b) cvalue.b = attenuation.b;
      }

      fvalue = (data->evalAnisotropy ? AiShaderEvalParamFlt(p_anisotropy) : data->anisotropy);
      
      AiShaderGlobalsSetVolumeScattering(sg, cvalue, fvalue);
   }

   // Emission
   if (!(sg->Rt & AI_RAY_SHADOW))
   {
      cvalue = (data->evalEmissionColor ? AiShaderEvalParamRGB(p_emission_color) : data->emissionColor);
      cvalue *= (data->evalEmissionWeight ? AiShaderEvalParamFlt(p_emission_weight) : data->emissionWeight);

      // Avoid possible field evaluation if weight * color is zero
      if (!AiColorIsZero(cvalue))
      {
         AtColor emission = AI_RGB_BLACK;

         switch (data->emissionSource)
         {
         case S_float_field:
            fvalue = 0.0f;
            if (AiVolumeSampleFlt(data->emissionField, data->interpolation, &fvalue))
            {
               emission.r = fvalue;
               emission.g = fvalue;
               emission.b = fvalue;
            }
            break;
         case S_color_field:
            AiVolumeSampleRGB(data->emissionField, data->interpolation, &emission);
            break;
         case S_parameter:
         default:
            emission = (data->evalEmission ? AiShaderEvalParamRGB(p_emission) : data->emission);
         }

         emission *= cvalue;
         
         AiColorClipToZero(emission);

         AiShaderGlobalsSetVolumeEmission(sg, emission);
      }
   }

   // Restore shader globals positions
   sg->P = oldP;
   sg->Po = oldPo;
}
