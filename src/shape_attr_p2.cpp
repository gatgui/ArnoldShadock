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

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrP2Mtd);

enum ShapeAttrP2Params
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterVec2(SSTR::_default, 0.0f, 0.0f);
   AiParameterEnum(SSTR::output_mode, AM_V, AttributeModeNames);
}

struct ShapeAttrP2Data
{
   AtString attribute;
   AttributeMode output_mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrP2Data());
   AddMemUsage<ShapeAttrP2Data>();
}

node_update
{
   ShapeAttrP2Data *data = (ShapeAttrP2Data*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->output_mode = (AttributeMode) AiNodeGetInt(node, SSTR::output_mode);
}

node_finish
{
   ShapeAttrP2Data *data = (ShapeAttrP2Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrP2Data>();
}

shader_evaluate
{
   ShapeAttrP2Data *data = (ShapeAttrP2Data*) AiNodeGetLocalData(node);
   
   sg->out.VEC2() = AI_P2_ZERO;
   
   if (data->output_mode == AM_V)
   {
      if (!AiUDataGetVec2(data->attribute, sg->out.VEC2()))
      {
         sg->out.VEC2() = AiShaderEvalParamVec2(p_default);
      }
   }
   else
   {
      AtVector2 dVdx, dVdy;
      
      if (!AiUDataGetDxyDerivativesVec2(data->attribute, dVdx, dVdx))
      {
         sg->out.VEC2() = AiShaderEvalParamVec2(p_default);
      }
      else
      {
         sg->out.VEC2() = (data->output_mode == AM_dVdx ? dVdx : dVdy);
      }
   }
}
