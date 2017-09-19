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

AI_SHADER_NODE_EXPORT_METHODS(WriteMsgsMtd);

enum WriteMsgsParams
{
   p_input = 0,
   p_bool_msg_name,
   p_bool_msg,
   p_int_msg_name,
   p_int_msg,
   p_float_msg_name,
   p_float_msg,
   p_vector2_msg_name,
   p_vector2_msg,
   p_vector_msg_name,
   p_vector_msg,
   p_rgb_msg_name,
   p_rgb_msg,
   p_rgba_msg_name,
   p_rgba_msg,
   p_eval_order
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterStr(SSTR::bool_msg_name, "");
   AiParameterBool("bool_msg", false);
   
   AiParameterStr(SSTR::int_msg_name, "");
   AiParameterInt("int_msg", 0);
   
   AiParameterStr(SSTR::float_msg_name, "");
   AiParameterFlt("float_msg", 0.0f);
   
   AiParameterStr(SSTR::vector2_msg_name, "");
   AiParameterVec2("point2_msg", 0.0f, 0.0f);
   
   AiParameterStr(SSTR::vector_msg_name, "");
   AiParameterVec("vector_msg", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::rgb_msg_name, "");
   AiParameterRGB("rgb_msg", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::rgba_msg_name, "");
   AiParameterRGBA("rgba_msg", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterEnum(SSTR::eval_order, EO_input_last, EvalOrderNames);
}

struct WriteMsgsData
{
   bool hasBool;
   bool hasInt;
   bool hasFloat;
   bool hasVector2;
   bool hasVector;
   bool hasRGB;
   bool hasRGBA;
   AtString boolName;
   AtString intName;
   AtString floatName;
   AtString vector2Name;
   AtString vectorName;
   AtString rgbName;
   AtString rgbaName;
   EvalOrder evalOrder;
};

node_initialize
{
   WriteMsgsData *data = new WriteMsgsData();
   AddMemUsage<WriteMsgsData>();
   
   data->hasBool = false;
   data->hasInt = false;
   data->hasFloat = false;
   data->hasVector2 = false;
   data->hasVector = false;
   data->hasRGB = false;
   data->hasRGBA = false;
   data->evalOrder = EO_input_last;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   WriteMsgsData *data = (WriteMsgsData*) AiNodeGetLocalData(node);
   
   data->evalOrder = (EvalOrder) AiNodeGetInt(node, SSTR::eval_order);
   
   data->boolName = AiNodeGetStr(node, SSTR::bool_msg_name);
   data->hasBool = !data->boolName.empty();
   
   data->intName = AiNodeGetStr(node, SSTR::int_msg_name);
   data->hasInt = !data->intName.empty();
   
   data->floatName = AiNodeGetStr(node, SSTR::float_msg_name);
   data->hasFloat = !data->floatName.empty();
   
   data->vector2Name = AiNodeGetStr(node, SSTR::vector2_msg_name);
   data->hasVector2 = !data->vector2Name.empty();
   
   data->vectorName = AiNodeGetStr(node, SSTR::vector_msg_name);
   data->hasVector = !data->vectorName.empty();
   
   data->rgbName = AiNodeGetStr(node, SSTR::rgb_msg_name);
   data->hasRGB = !data->rgbName.empty();
   
   data->rgbaName = AiNodeGetStr(node, SSTR::rgba_msg_name);
   data->hasRGBA = !data->rgbaName.empty();
}

node_finish
{
   WriteMsgsData *data = (WriteMsgsData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<WriteMsgsData>();
}

shader_evaluate
{
   WriteMsgsData *data = (WriteMsgsData*) AiNodeGetLocalData(node);
   
   if (data->evalOrder == EO_input_first)
   {
      sg->out.RGBA() = AiShaderEvalParamRGBA(p_input);
   }
   
   if (data->hasBool)
   {
      AiStateSetMsgBool(data->boolName, AiShaderEvalParamBool(p_bool_msg));
   }
   if (data->hasInt)
   {
      AiStateSetMsgInt(data->intName, AiShaderEvalParamInt(p_int_msg));
   }
   if (data->hasFloat)
   {
      AiStateSetMsgFlt(data->floatName, AiShaderEvalParamFlt(p_float_msg));
   }
   if (data->hasVector2)
   {
      AiStateSetMsgVec2(data->vector2Name, AiShaderEvalParamVec2(p_vector2_msg));
   }
   if (data->hasVector)
   {
      AiStateSetMsgVec(data->vectorName, AiShaderEvalParamVec(p_vector_msg));
   }
   if (data->hasRGB)
   {
      AiStateSetMsgRGB(data->rgbName, AiShaderEvalParamRGB(p_rgb_msg));
   }
   if (data->hasRGBA)
   {
      AiStateSetMsgRGBA(data->rgbaName, AiShaderEvalParamRGBA(p_rgba_msg));
   }
   
   if (data->evalOrder == EO_input_last)
   {
      sg->out.RGBA() = AiShaderEvalParamRGBA(p_input);
   }
}
