#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeFMtd);

enum SampleVolumeFParams
{
   p_field = 0,
   p_interpolation,
   p_P,
   p_P_space,
   p_P_is_offset,
   p_pre_multiply,
   p_pre_offset,
   p_blend_mode,
   p_blend,
   p_blend_min,
   p_blend_max,
   p_bias,
   p_gain,
   p_multiply,
   p_offset,
   p_default
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
   AiParameterStr(SSTR::field, "");
   AiParameterEnum(SSTR::interpolation, VI_linear, VolumeInterpNames);
   AiParameterPnt(SSTR::P, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::P_space, S_world, SpaceNames);
   AiParameterBool(SSTR::P_is_offset, true);
   AiParameterFlt(SSTR::pre_multiply, 1.0f);
   AiParameterFlt(SSTR::pre_offset, 0.0f);
   AiParameterEnum(SSTR::blend_mode, BM_none, BlendModeNames);
   AiParameterFlt(SSTR::blend, 1.0f);
   AiParameterFlt(SSTR::blend_min, 0.0f);
   AiParameterFlt(SSTR::blend_max, 1.0f);
   AiParameterFlt(SSTR::bias, 0.5f);
   AiParameterFlt(SSTR::gain, 0.5f);
   AiParameterFlt(SSTR::multiply, 1.0f);
   AiParameterFlt(SSTR::offset, 0.0f);
   AiParameterFlt(SSTR::_default, 0.0f);
}

struct SampleVolumeFData
{
   AtString field;
   int interpolation;
   bool evalP;
   bool ignoreP;
   AtPoint P;
   Space space;
   bool isOffset;
   bool evalDefault;
   float _default;
   float bias;
   float gain;
   float preMult;
   float preAdd;
   BlendMode blendMode;
   float postMult;
   float postAdd;
};

node_initialize
{
   AiNodeSetLocalData(node, new SampleVolumeFData());
   AddMemUsage<SampleVolumeFData>();
}

node_update
{
   SampleVolumeFData *data = (SampleVolumeFData*) AiNodeGetLocalData(node);
   
   data->field = AiNodeGetStr(node, SSTR::field);
   data->interpolation = AiNodeGetInt(node, SSTR::interpolation);
   data->evalDefault = AiNodeIsLinked(node, SSTR::_default);
   if (!data->evalDefault)
   {
      data->_default = AiNodeGetFlt(node, SSTR::_default);
   }
   data->preMult = AiNodeGetFlt(node, SSTR::pre_multiply);
   data->preAdd = AiNodeGetFlt(node, SSTR::pre_offset);
   data->blendMode = (BlendMode) AiNodeGetInt(node, SSTR::blend_mode);
   data->bias = AiNodeGetFlt(node, SSTR::bias);
   data->gain = AiNodeGetFlt(node, SSTR::gain);
   data->postMult = AiNodeGetFlt(node, SSTR::multiply);
   data->postAdd = AiNodeGetFlt(node, SSTR::offset);
   data->space = (Space) AiNodeGetInt(node, SSTR::P_space);
   data->isOffset = AiNodeGetBool(node, SSTR::P_is_offset);
   data->evalP = AiNodeIsLinked(node, SSTR::P);
   if (!data->evalP)
   {
      data->P = AiNodeGetPnt(node, SSTR::P);
      data->ignoreP = (data->isOffset && AiV3IsZero(data->P));
   }
   else
   {
      data->ignoreP = false;
   }
}

node_finish
{
   SampleVolumeFData *data = (SampleVolumeFData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<SampleVolumeFData>();
}

shader_evaluate
{
   SampleVolumeFData *data = (SampleVolumeFData*) AiNodeGetLocalData(node);
   
   AtPoint oldP = sg->P;
   AtPoint oldPo = sg->Po;
   
   if (!data->ignoreP)
   {
      AtPoint P;
      bool updateP = true;
      
      if (data->evalP)
      {
         P = AiShaderEvalParamPnt(p_P);
         updateP = (!data->isOffset || !AiV3IsZero(P));
      }
      else
      {
         P = data->P;
      }

      if (updateP)
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
   }

   float out = 0.0f;

   if (!AiVolumeSampleFlt(data->field, data->interpolation, &out))
   {
      sg->out.FLT = (data->evalDefault ? AiShaderEvalParamFlt(p_default) : data->_default);
   }
   else
   {
      // Pre mult/add
      out = data->preAdd + data->preMult * out;
      
      // Blend
      switch (data->blendMode)
      {
      case BM_min_field:
         {
            float blend = AiShaderEvalParamFlt(p_blend);
            if (blend < 1.0f)
            {
               float bmin = AiShaderEvalParamFlt(p_blend_min);
               out = bmin + blend * (out - bmin);
            }
         }
         break;
      case BM_field_max:
         {
            float blend = AiShaderEvalParamFlt(p_blend);
            if (blend > 0.0f)
            {
               float bmax = AiShaderEvalParamFlt(p_blend_max);
               out = out + blend * (bmax - out);
            }
         }
         break;
      case BM_field_as_blender:
         {
            float bmin = AiShaderEvalParamFlt(p_blend_min);
            float bmax = AiShaderEvalParamFlt(p_blend_max);
            out = bmin + out * (bmax - bmin);
         }
      default:
         break;
      }
      
      // Gain/Bias, Post mult/add
      sg->out.FLT = data->postAdd + data->postMult * GAIN(BIAS(out, data->bias), data->gain);
   }

   sg->P = oldP;
   sg->Po = oldPo;
}
