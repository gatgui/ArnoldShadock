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

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrVMtd);

enum ShapeAttrVParams
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterVec(SSTR::_default, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::output_mode, AM_V, AttributeModeNames);
}

struct ShapeAttrVData
{
   AtString attribute;
   AttributeMode output_mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrVData());
   AddMemUsage<ShapeAttrVData>();
}

node_update
{
   ShapeAttrVData *data = (ShapeAttrVData*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->output_mode = (AttributeMode) AiNodeGetInt(node, SSTR::output_mode);
}

node_finish
{
   ShapeAttrVData *data = (ShapeAttrVData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrVData>();
}

shader_evaluate
{
   ShapeAttrVData *data = (ShapeAttrVData*) AiNodeGetLocalData(node);
   
   sg->out.VEC() = AI_V3_ZERO;

   if (data->output_mode == AM_V)
   {
      if (!AiUDataGetVec(data->attribute, sg->out.VEC()))
      {
         sg->out.VEC() = AiShaderEvalParamVec(p_default);
      }
   }
   else
   {
      AtVector dVdx, dVdy;
      
      if (!AiUDataGetDxyDerivativesVec(data->attribute, dVdx, dVdy))
      {
         sg->out.VEC() = AiShaderEvalParamVec(p_default);
      }
      else
      {
         sg->out.VEC() = (data->output_mode == AM_dVdx ? dVdx : dVdy);
      }
   }
}
