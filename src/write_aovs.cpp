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

AI_SHADER_NODE_EXPORT_METHODS(WriteAOVsMtd);

enum WriteAOVsParams
{
   p_input = 0,
   p_bool_aov_name,
   p_bool_aov,
   p_int_aov_name,
   p_int_aov,
   p_float_aov_name,
   p_float_aov,
   p_point2_aov_name,
   p_point2_aov,
   p_point_aov_name,
   p_point_aov,
   p_vector_aov_name,
   p_vector_aov,
   p_rgb_aov_name,
   p_rgb_aov,
   p_rgba_aov_name,
   p_rgba_aov,
   p_blend_opacity,
   p_eval_order
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterStr(SSTR::bool_aov_name, "");
   AiParameterBool("bool_aov", false);
   
   AiParameterStr(SSTR::int_aov_name, "");
   AiParameterInt("int_aov", 0);
   
   AiParameterStr(SSTR::float_aov_name, "");
   AiParameterFlt("float_aov", 0.0f);
   
   AiParameterStr(SSTR::point2_aov_name, "");
   AiParameterPnt2("point2_aov", 0.0f, 0.0f);
   
   AiParameterStr(SSTR::point_aov_name, "");
   AiParameterPnt("point_aov", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::vector_aov_name, "");
   AiParameterVec("vector_aov", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::rgb_aov_name, "");
   AiParameterRGB("rgb_aov", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::rgba_aov_name, "");
   AiParameterRGBA("rgba_aov", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterBool(SSTR::blend_opacity, true);
   
   AiParameterEnum(SSTR::eval_order, EO_input_last, EvalOrderNames);
}

struct WriteAOVsData
{
   bool hasBool;
   bool hasInt;
   bool hasFloat;
   bool hasPoint2;
   bool hasPoint;
   bool hasVector;
   bool hasRGB;
   bool hasRGBA;
   AtString boolName;
   AtString intName;
   AtString floatName;
   AtString point2Name;
   AtString pointName;
   AtString vectorName;
   AtString rgbName;
   AtString rgbaName;
   EvalOrder evalOrder;
};

node_initialize
{
   WriteAOVsData *data = new WriteAOVsData();
   AddMemUsage<WriteAOVsData>();
   
   data->hasBool = false;
   data->hasInt = false;
   data->hasFloat = false;
   data->hasPoint2 = false;
   data->hasPoint = false;
   data->hasVector = false;
   data->hasRGB = false;
   data->hasRGBA = false;
   data->evalOrder = EO_input_last;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   WriteAOVsData *data = (WriteAOVsData*) AiNodeGetLocalData(node);
   
   bool blend = AiNodeGetBool(node, SSTR::blend_opacity);
   
   data->evalOrder = (EvalOrder) AiNodeGetInt(node, SSTR::eval_order);
   
   data->hasBool = false;
   data->boolName = AiNodeGetStr(node, SSTR::bool_aov_name);
   if (!data->boolName.empty())
   {
      data->hasBool = AiAOVRegister(data->boolName, AI_TYPE_BOOLEAN, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
   }
   
   data->hasInt = false;
   data->intName = AiNodeGetStr(node, SSTR::int_aov_name);
   if (!data->intName.empty())
   {
      data->hasInt = AiAOVRegister(data->intName, AI_TYPE_INT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
   }
   
   data->hasFloat = false;
   data->floatName = AiNodeGetStr(node, SSTR::float_aov_name);
   if (!data->floatName.empty())
   {
      AiAOVRegister(data->floatName, AI_TYPE_FLOAT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasFloat = true;
   }
   
   data->hasPoint2 = false;
   data->point2Name = AiNodeGetStr(node, SSTR::point2_aov_name);
   if (!data->point2Name.empty())
   {
      data->hasPoint2 = AiAOVRegister(data->point2Name, AI_TYPE_POINT2, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
   }
   
   data->hasPoint = false;
   data->pointName = AiNodeGetStr(node, SSTR::point_aov_name);
   if (!data->pointName.empty())
   {
      data->hasPoint = AiAOVRegister(data->pointName, AI_TYPE_POINT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
   }
   
   data->hasVector = false;
   data->vectorName = AiNodeGetStr(node, SSTR::vector_aov_name);
   if (!data->vectorName.empty())
   {
      data->hasVector = AiAOVRegister(data->vectorName, AI_TYPE_VECTOR, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
   }
   
   data->hasRGB = false;
   data->rgbName = AiNodeGetStr(node, SSTR::rgb_aov_name);
   if (!data->rgbName.empty())
   {
      data->hasRGB = AiAOVRegister(data->rgbName, AI_TYPE_RGB, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
   }
   
   data->hasRGBA = false;
   data->rgbaName = AiNodeGetStr(node, SSTR::rgba_aov_name);
   if (!data->rgbaName.empty())
   {
      data->hasRGBA = AiAOVRegister(data->rgbaName, AI_TYPE_RGBA, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
   }
}

node_finish
{
   WriteAOVsData *data = (WriteAOVsData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<WriteAOVsData>();
}

shader_evaluate
{
   WriteAOVsData *data = (WriteAOVsData*) AiNodeGetLocalData(node);
   
   if (data->evalOrder == EO_input_first)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
   
   if ((sg->Rt & AI_RAY_CAMERA))
   {
      if (data->hasBool && AiAOVEnabled(data->boolName, AI_TYPE_BOOLEAN))
      {
         AiAOVSetBool(sg, data->boolName, AiShaderEvalParamBool(p_bool_aov));
      }
      if (data->hasInt && AiAOVEnabled(data->intName, AI_TYPE_INT))
      {
         AiAOVSetInt(sg, data->intName, AiShaderEvalParamInt(p_int_aov));
      }
      if (data->hasFloat && AiAOVEnabled(data->floatName, AI_TYPE_FLOAT))
      {
         AiAOVSetFlt(sg, data->floatName, AiShaderEvalParamFlt(p_float_aov));
      }
      if (data->hasPoint2 && AiAOVEnabled(data->point2Name, AI_TYPE_POINT2))
      {
         AiAOVSetPnt2(sg, data->point2Name, AiShaderEvalParamPnt2(p_point2_aov));
      }
      if (data->hasPoint && AiAOVEnabled(data->pointName, AI_TYPE_POINT))
      {
         AiAOVSetPnt(sg, data->pointName, AiShaderEvalParamPnt(p_point_aov));
      }
      if (data->hasVector && AiAOVEnabled(data->vectorName, AI_TYPE_VECTOR))
      {
         AiAOVSetVec(sg, data->vectorName, AiShaderEvalParamVec(p_vector_aov));
      }
      if (data->hasRGB && AiAOVEnabled(data->rgbName, AI_TYPE_RGB))
      {
         AiAOVSetRGB(sg, data->rgbName, AiShaderEvalParamRGB(p_rgb_aov));
      }
      if (data->hasRGBA && AiAOVEnabled(data->rgbaName, AI_TYPE_RGBA))
      {
         AiAOVSetRGBA(sg, data->rgbaName, AiShaderEvalParamRGBA(p_rgba_aov));
      }
   }
   
   if (data->evalOrder == EO_input_last)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}
