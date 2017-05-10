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

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrC4Mtd);

enum ShapeAttrC4Params
{
   p_attribute = 0,
   p_default,
   p_output_mode
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterRGBA(SSTR::_default, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterEnum(SSTR::output_mode, AM_V, AttributeModeNames);
}

struct ShapeAttrC4Data
{
   AtString attribute;
   AttributeMode output_mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrC4Data());
   AddMemUsage<ShapeAttrC4Data>();
}

node_update
{
   ShapeAttrC4Data *data = (ShapeAttrC4Data*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->output_mode = (AttributeMode) AiNodeGetInt(node, SSTR::output_mode);
}

node_finish
{
   ShapeAttrC4Data *data = (ShapeAttrC4Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrC4Data>();
}

shader_evaluate
{
   ShapeAttrC4Data *data = (ShapeAttrC4Data*) AiNodeGetLocalData(node);

   sg->out.RGBA = AI_RGBA_BLACK;
   
   if (data->output_mode == AM_V)
   {
      if (!AiUDataGetRGBA(data->attribute, &(sg->out.RGBA)))
      {
         if (!AiUDataGetRGB(data->attribute, &(sg->out.RGB)))
         {
            sg->out.RGBA = AiShaderEvalParamRGBA(p_default);
         }
         else
         {
            sg->out.RGBA.a = 1.0f;
         }
      }
   }
   else
   {
      AtRGBA dVdx, dVdy;
      
      if (!AiUDataGetDxyDerivativesRGBA(data->attribute, &dVdx, &dVdy))
      {
         if (!AiUDataGetDxyDerivativesRGB(data->attribute, &dVdx.rgb(), &dVdy.rgb()))
         {
            sg->out.RGBA = AiShaderEvalParamRGBA(p_default);
            return;
         }
         
         dVdx.a = 1.0f;
         dVdy.a = 1.0f;
      }
      
      sg->out.RGBA = (data->output_mode == AM_dVdx ? dVdx : dVdy);
   }
}
