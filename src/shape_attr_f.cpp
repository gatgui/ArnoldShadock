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

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrFMtd);

enum ShapeAttrFParams
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterFlt(SSTR::_default, 0.0f);
   AiParameterEnum(SSTR::output_mode, AM_V, AttributeModeNames);
}

struct ShapeAttrFData
{
   AtString attribute;
   AttributeMode output_mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrFData());
   AddMemUsage<ShapeAttrFData>();
}

node_update
{
   ShapeAttrFData *data = (ShapeAttrFData*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->output_mode = (AttributeMode) AiNodeGetInt(node, SSTR::output_mode);
}

node_finish
{
   ShapeAttrFData *data = (ShapeAttrFData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrFData>();
}

shader_evaluate
{
   ShapeAttrFData *data = (ShapeAttrFData*) AiNodeGetLocalData(node);
   
   if (data->output_mode == AM_V)
   {
      // Int, UInt, Byte?
      if (!AiUDataGetFlt(data->attribute, sg->out.FLT()))
      {
         sg->out.FLT() = AiShaderEvalParamFlt(p_default);
      }
   }
   else
   {
      float dVdx = 0.0f;
      float dVdy = 0.0f;
      
      if (!AiUDataGetDxyDerivativesFlt(data->attribute, dVdx, dVdx))
      {
         sg->out.FLT() = AiShaderEvalParamFlt(p_default);
      }
      else
      {
         sg->out.FLT() = (data->output_mode == AM_dVdx ? dVdx : dVdy);
      }
   }
}
