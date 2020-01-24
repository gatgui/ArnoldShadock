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

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrIMtd);

enum ShapeAttrIParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterInt(SSTR::_default, 0);
}

struct ShapeAttrIData
{
   AtString attribute;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrIData());
   AddMemUsage<ShapeAttrIData>();
}

node_update
{
   ShapeAttrIData *data = (ShapeAttrIData*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
}

node_finish
{
   ShapeAttrIData *data = (ShapeAttrIData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrIData>();
}

shader_evaluate
{
   ShapeAttrIData *data = (ShapeAttrIData*) AiNodeGetLocalData(node);
   
   if (!AiUDataGetInt(data->attribute, sg->out.INT()))
   {
      uint8_t b = 0;
      unsigned int ui = 0;

      if (AiUDataGetByte(data->attribute, b))
      {
         sg->out.INT() = int(b);
      }
      else if (AiUDataGetUInt(data->attribute, ui))
      {
         sg->out.INT() = int(ui);
      }
      else
      {
         sg->out.INT() = AiShaderEvalParamInt(p_default);
      }
   }
}
