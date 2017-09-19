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

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeVMtd);

enum SampleVolumeVParams
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
   AiParameterVec(SSTR::P, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::P_space, S_world, SpaceNames);
   AiParameterBool(SSTR::P_is_offset, true);
   AiParameterVec(SSTR::pre_multiply, 1.0f, 1.0f, 1.0f);
   AiParameterVec(SSTR::pre_offset, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::blend_mode, BM_none, BlendModeNames);
   AiParameterFlt(SSTR::blend, 1.0f);
   AiParameterVec(SSTR::blend_min, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::blend_max, 1.0f, 1.0f, 1.0f);
   AiParameterFlt(SSTR::bias, 0.5f);
   AiParameterFlt(SSTR::gain, 0.5f);
   AiParameterVec(SSTR::multiply, 1.0f, 1.0f, 1.0f);
   AiParameterVec(SSTR::offset, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::_default, 0.0f, 0.0f, 0.0f);
}

struct SampleVolumeVData
{
   AtString field;
   int interpolation;
   bool evalP;
   bool ignoreP;
   AtVector P;
   Space space;
   bool isOffset;
   bool evalDefault;
   AtVector _default;
   float bias;
   float gain;
   AtVector preMult;
   AtVector preAdd;
   BlendMode blendMode;
   AtVector postMult;
   AtVector postAdd;
};

node_initialize
{
   AiNodeSetLocalData(node, new SampleVolumeVData());
   AddMemUsage<SampleVolumeVData>();
}

node_update
{
   SampleVolumeVData *data = (SampleVolumeVData*) AiNodeGetLocalData(node);
   
   data->field = AiNodeGetStr(node, SSTR::field);
   data->interpolation = AiNodeGetInt(node, SSTR::interpolation);
   data->evalDefault = AiNodeIsLinked(node, SSTR::_default);
   if (!data->evalDefault)
   {
      data->_default = AiNodeGetVec(node, SSTR::_default);
   }
   data->preMult = AiNodeGetVec(node, SSTR::pre_multiply);
   data->preAdd = AiNodeGetVec(node, SSTR::pre_offset);
   data->blendMode = (BlendMode) AiNodeGetInt(node, SSTR::blend_mode);
   data->bias = AiNodeGetFlt(node, SSTR::bias);
   data->gain = AiNodeGetFlt(node, SSTR::gain);
   data->postMult = AiNodeGetVec(node, SSTR::multiply);
   data->postAdd = AiNodeGetVec(node, SSTR::offset);
   data->space = (Space) AiNodeGetInt(node, SSTR::P_space);
   data->isOffset = AiNodeGetBool(node, SSTR::P_is_offset);
   data->evalP = AiNodeIsLinked(node, SSTR::P);
   if (!data->evalP)
   {
      data->P = AiNodeGetVec(node, SSTR::P);
      data->ignoreP = (data->isOffset && AiV3IsSmall(data->P));
   }
   else
   {
      data->ignoreP = false;
   }
}

node_finish
{
   SampleVolumeVData *data = (SampleVolumeVData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<SampleVolumeVData>();
}

shader_evaluate
{
   SampleVolumeVData *data = (SampleVolumeVData*) AiNodeGetLocalData(node);
   
   AtVector oldP = sg->P;
   AtVector oldPo = sg->Po;
   
   if (!data->ignoreP)
   {
      AtVector P;
      bool updateP = true;
      
      if (data->evalP)
      {
         P = AiShaderEvalParamVec(p_P);
         updateP = (!data->isOffset || !AiV3IsSmall(P));
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
            sg->P = AiM4PointByMatrixMult(sg->M, sg->Po);
            break;
         case S_world:
         default:
            sg->P = (data->isOffset ? (sg->P + P) : P);
            sg->Po = AiM4PointByMatrixMult(sg->Minv, sg->P);
         }
      }
   }

   AtVector out = AI_V3_ZERO;

   if (!AiVolumeSampleVec(data->field, data->interpolation, &out))
   {
      sg->out.VEC() = (data->evalDefault ? AiShaderEvalParamVec(p_default) : data->_default);
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
               AtVector bmin = AiShaderEvalParamVec(p_blend_min);
               out = bmin + blend * (out - bmin);
            }
         }
         break;
      case BM_field_max:
         {
            float blend = AiShaderEvalParamFlt(p_blend);
            if (blend > 0.0f)
            {
               AtVector bmax = AiShaderEvalParamVec(p_blend_max);
               out = out + blend * (bmax - out);
            }
         }
         break;
      case BM_field_as_blender:
         {
            AtVector bmin = AiShaderEvalParamVec(p_blend_min);
            AtVector bmax = AiShaderEvalParamVec(p_blend_max);
            out.x = bmin.x + out.x * (bmax.x - bmin.x);
            out.y = bmin.y + out.y * (bmax.y - bmin.y);
            out.z = bmin.z + out.z * (bmax.z - bmin.z);
         }
      default:
         break;
      }
      
      // Gain/Bias
      out.x = AiGain(AiBias(out.x, data->bias), data->gain);
      out.y = AiGain(AiBias(out.y, data->bias), data->gain);
      out.z = AiGain(AiBias(out.z, data->bias), data->gain);
      
      // Post mult/add
      sg->out.VEC() = data->postAdd + data->postMult * out;
   }

   sg->P = oldP;
   sg->Po = oldPo;
}
