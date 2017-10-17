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

AI_SHADER_NODE_EXPORT_METHODS(GradeMtd);

enum GradeParams
{
   p_input = 0,
   p_in_black,
   p_in_white,
   p_out_black,
   p_out_white,
   p_saturation_scale,
   p_hue_shift,
   p_bias,
   p_gain,
   p_gamma,
   p_exposure
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterRGB(SSTR::in_black, 0.0f, 0.0f, 0.0f);
   AiParameterRGB(SSTR::in_white, 1.0f, 1.0f, 1.0f);
   AiParameterRGB(SSTR::out_black, 0.0f, 0.0f, 0.0f);
   AiParameterRGB(SSTR::out_white, 1.0f, 1.0f, 1.0f);
   AiParameterFlt(SSTR::saturation_scale, 1.0f);
   AiParameterFlt(SSTR::hue_shift, 0.5f);
   AiParameterFlt(SSTR::bias, 0.5f);
   AiParameterFlt(SSTR::gain, 0.5f);
   AiParameterFlt(SSTR::gamma, 1.0f);
   AiParameterFlt(SSTR::exposure, 0.0f);
}

struct GradeData
{
   AtRGB pre_offset;
   AtRGB scale;
   AtRGB post_offset;
};

node_initialize
{
   AiNodeSetLocalData(node, new GradeData());
   AddMemUsage<GradeData>();
}

node_update
{
   GradeData *data = (GradeData*) AiNodeGetLocalData(node);
   AtRGB ib = AiNodeGetRGB(node, SSTR::in_black);
   AtRGB iw = AiNodeGetRGB(node, SSTR::in_white);
   AtRGB ob = AiNodeGetRGB(node, SSTR::out_black);
   AtRGB ow = AiNodeGetRGB(node, SSTR::out_white);
   AtRGB id = iw - ib;
   AtRGB od = ow - ob;
   data->pre_offset = -ib;
   data->scale.r = (fabsf(id.r) > AI_EPSILON ? (od.r / id.r) : 1.0f);
   data->scale.g = (fabsf(id.g) > AI_EPSILON ? (od.g / id.g) : 1.0f);
   data->scale.b = (fabsf(id.b) > AI_EPSILON ? (od.b / id.b) : 1.0f);
   data->post_offset = ob;
}

node_finish
{
   GradeData *data = (GradeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<GradeData>();
}

shader_evaluate
{
   GradeData *data = (GradeData*) AiNodeGetLocalData(node);

   AtRGB input = AiShaderEvalParamRGB(p_input);
   float saturation_scale = AiShaderEvalParamFlt(p_saturation_scale);
   float hue_shift = AiShaderEvalParamFlt(p_hue_shift);
   float bias = AiShaderEvalParamFlt(p_bias);
   float gain = AiShaderEvalParamFlt(p_gain);
   float gamma = AiShaderEvalParamFlt(p_gamma);
   float exposure = AiShaderEvalParamFlt(p_exposure);

   gmath::RGB output;

   output.r = ((input.r + data->pre_offset.r) * data->scale.r) + data->post_offset.r;
   output.g = ((input.g + data->pre_offset.g) * data->scale.g) + data->post_offset.g;
   output.b = ((input.b + data->pre_offset.b) * data->scale.b) + data->post_offset.b;

   if (saturation_scale < (1.0f - AI_EPSILON) ||
       saturation_scale > (1.0f + AI_EPSILON) ||
       hue_shift < (0.5f - AI_EPSILON) ||
       hue_shift > (0.5f + AI_EPSILON))
   {
      gmath::HSV hsv = gmath::RGBtoHSV(output);

      hsv.h += hue_shift;
      hsv.h = hsv.h - floorf(hsv.h);

      hsv.s *= saturation_scale;

      output = gmath::HSVtoRGB(hsv);
   }

   output.r = AiGain(AiBias(output.r, bias), gain);
   output.g = AiGain(AiBias(output.g, bias), gain);
   output.b = AiGain(AiBias(output.b, bias), gain);

   if (gamma < (1.0f - AI_EPSILON) || gamma > (1.0f + AI_EPSILON))
   {
      output.r = powf(output.r, gamma);
      output.g = powf(output.g, gamma);
      output.b = powf(output.b, gamma);
   }

   if (exposure < -AI_EPSILON || exposure > AI_EPSILON)
   {
      output *= powf(2.0f, exposure);
   }

   sg->out.RGB().r = output.r;
   sg->out.RGB().g = output.g;
   sg->out.RGB().b = output.b;
}
