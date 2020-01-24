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

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrC3Mtd);

enum ShapeAttrC3Params
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterRGB(SSTR::_default, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::output_mode, AM_V, AttributeModeNames);
}

struct ShapeAttrC3Data
{
   AtString attribute;
   AttributeMode output_mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrC3Data());
   AddMemUsage<ShapeAttrC3Data>();
}

node_update
{
   ShapeAttrC3Data *data = (ShapeAttrC3Data*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->output_mode = (AttributeMode) AiNodeGetInt(node, SSTR::output_mode);
}

node_finish
{
   ShapeAttrC3Data *data = (ShapeAttrC3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrC3Data>();
}

shader_evaluate
{
   ShapeAttrC3Data *data = (ShapeAttrC3Data*) AiNodeGetLocalData(node);
   
   sg->out.RGB() = AI_RGB_BLACK;

   if (data->output_mode == AM_V)
   {
      if (!AiUDataGetRGB(data->attribute, sg->out.RGB()) &&
          !AiUDataGetRGBA(data->attribute, sg->out.RGBA()))
      {
         sg->out.RGB() = AiShaderEvalParamRGB(p_default);
      }
   }
   else
   {
      AtRGBA dVdx, dVdy;
      
      if (!AiUDataGetDxyDerivativesRGB(data->attribute, dVdx.rgb(), dVdy.rgb()) &&
          !AiUDataGetDxyDerivativesRGBA(data->attribute, dVdx, dVdy))
      {
         sg->out.RGB() = AiShaderEvalParamRGB(p_default);
      }
      else
      {
         sg->out.RGBA() = (data->output_mode == AM_dVdx ? dVdx : dVdy);
      }
   }
}
