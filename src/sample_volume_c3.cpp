/*
MIT License
Copyright (c) 2014~ Gaetan Guidet
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeC3Mtd);

enum SampleVolumeC3Params
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
   AiParameterRGB(SSTR::pre_multiply, 1.0f, 1.0f, 1.0f);
   AiParameterRGB(SSTR::pre_offset, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::blend_mode, BM_none, BlendModeNames);
   AiParameterFlt(SSTR::blend, 1.0f);
   AiParameterRGB(SSTR::blend_min, 0.0f, 0.0f, 0.0f);
   AiParameterRGB(SSTR::blend_max, 1.0f, 1.0f, 1.0f);
   AiParameterFlt(SSTR::bias, 0.5f);
   AiParameterFlt(SSTR::gain, 0.5f);
   AiParameterRGB(SSTR::multiply, 1.0f, 1.0f, 1.0f);
   AiParameterRGB(SSTR::offset, 0.0f, 0.0f, 0.0f);
   AiParameterRGB(SSTR::_default, 0.0f, 0.0f, 0.0f);
}

struct SampleVolumeC3Data
{
   AtString field;
   int interpolation;
   bool evalP;
   bool ignoreP;
   AtPoint P;
   Space space;
   bool isOffset;
   bool evalDefault;
   AtColor _default;
   float bias;
   float gain;
   AtColor preMult;
   AtColor preAdd;
   BlendMode blendMode;
   AtColor postMult;
   AtColor postAdd;
};

node_initialize
{
   AiNodeSetLocalData(node, new SampleVolumeC3Data());
   AddMemUsage<SampleVolumeC3Data>();
}

node_update
{
   SampleVolumeC3Data *data = (SampleVolumeC3Data*) AiNodeGetLocalData(node);
   
   data->field = AiNodeGetStr(node, SSTR::field);
   data->interpolation = AiNodeGetInt(node, SSTR::interpolation);
   data->evalDefault = AiNodeIsLinked(node, SSTR::_default);
   if (!data->evalDefault)
   {
      data->_default = AiNodeGetRGB(node, SSTR::_default);
   }
   data->preMult = AiNodeGetRGB(node, SSTR::pre_multiply);
   data->preAdd = AiNodeGetRGB(node, SSTR::pre_offset);
   data->blendMode = (BlendMode) AiNodeGetInt(node, SSTR::blend_mode);
   data->bias = AiNodeGetFlt(node, SSTR::bias);
   data->gain = AiNodeGetFlt(node, SSTR::gain);
   data->postMult = AiNodeGetRGB(node, SSTR::multiply);
   data->postAdd = AiNodeGetRGB(node, SSTR::offset);
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
   SampleVolumeC3Data *data = (SampleVolumeC3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<SampleVolumeC3Data>();
}

shader_evaluate
{
   SampleVolumeC3Data *data = (SampleVolumeC3Data*) AiNodeGetLocalData(node);
   
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

   AtColor out = AI_RGB_BLACK;

   if (!AiVolumeSampleRGB(data->field, data->interpolation, &out))
   {
      sg->out.RGB = (data->evalDefault ? AiShaderEvalParamRGB(p_default) : data->_default);
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
               AtColor bmin = AiShaderEvalParamRGB(p_blend_min);
               out = bmin + blend * (out - bmin);
            }
         }
         break;
      case BM_field_max:
         {
            float blend = AiShaderEvalParamFlt(p_blend);
            if (blend > 0.0f)
            {
               AtColor bmax = AiShaderEvalParamRGB(p_blend_max);
               out = out + blend * (bmax - out);
            }
         }
         break;
      case BM_field_as_blender:
         {
            AtColor bmin = AiShaderEvalParamRGB(p_blend_min);
            AtColor bmax = AiShaderEvalParamRGB(p_blend_max);
            out.r = bmin.r + out.r * (bmax.r - bmin.r);
            out.g = bmin.g + out.g * (bmax.g - bmin.g);
            out.b = bmin.b + out.b * (bmax.b - bmin.b);
         }
      default:
         break;
      }
      
      // Gain/Bias
      out.r = GAIN(BIAS(out.r, data->bias), data->gain);
      out.g = GAIN(BIAS(out.g, data->bias), data->gain);
      out.b = GAIN(BIAS(out.b, data->bias), data->gain);
      
      // Post mult/add
      sg->out.RGB = data->postAdd + data->postMult * out;
   }

   sg->P = oldP;
   sg->Po = oldPo;
}
