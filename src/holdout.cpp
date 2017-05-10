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

AI_SHADER_NODE_EXPORT_METHODS(HoldoutMtd);

enum HoldoutParams
{
   p_input = 0,
   p_enable_attr,
   p_default_enable,
   p_color_attr,
   p_default_color
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterStr(SSTR::enable_attr, "");
   AiParameterBool(SSTR::default_enable, true);
   AiParameterStr(SSTR::color_attr, "");
   AiParameterRGBA(SSTR::default_color, 0.0f, 0.0f, 0.0f, 0.0f);
}

struct HoldoutData
{
   AtString enableAttr;
   bool evalDefaultEnable;
   bool defaultEnable;
   
   AtString colorAttr;
   bool evalDefaultColor;
   AtRGBA defaultColor;
};

node_initialize
{
   AiNodeSetLocalData(node, new HoldoutData());
   AddMemUsage<HoldoutData>();
}

node_update
{
   HoldoutData *data = (HoldoutData*) AiNodeGetLocalData(node);
   
   data->enableAttr = AiNodeGetStr(node, SSTR::enable_attr);
   
   data->evalDefaultEnable = AiNodeIsLinked(node, SSTR::default_enable);
   if (!data->evalDefaultEnable)
   {
      data->defaultEnable = AiNodeGetBool(node, SSTR::default_enable);
   }
   
   data->colorAttr = AiNodeGetStr(node, SSTR::color_attr);
   
   data->evalDefaultColor = AiNodeIsLinked(node, SSTR::default_color);
   if (!data->evalDefaultColor)
   {
      data->defaultColor = AiNodeGetRGBA(node, SSTR::default_color);
   }
}

node_finish
{
   HoldoutData *data = (HoldoutData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<HoldoutData>();
}

shader_evaluate
{
   HoldoutData *data = (HoldoutData*) AiNodeGetLocalData(node);
   
   bool holdout = false;
   
   if ((sg->Rt & AI_RAY_CAMERA) != 0)
   {
      if (data->enableAttr.empty() || !AiUDataGetBool(data->enableAttr, &holdout))
      {
         holdout = (data->evalDefaultEnable ? AiShaderEvalParamBool(p_default_enable) : data->defaultEnable);
      }
   }
   
   if (holdout)
   {
      if (data->colorAttr.empty() || !AiUDataGetRGBA(data->colorAttr, &(sg->out.RGBA)))
      {
         sg->out.RGBA = (data->evalDefaultColor ? AiShaderEvalParamRGBA(p_default_color) : data->defaultColor);
      }
   }
   else
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}

