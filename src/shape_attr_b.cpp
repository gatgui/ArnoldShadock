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

AI_SHADER_NODE_EXPORT_METHODS(ShapeAttrBMtd);

enum ShapeAttrBParams
{
   p_attribute = 0,
   p_default
};

node_parameters
{
   AiParameterStr(SSTR::attribute, "");
   AiParameterBool(SSTR::_default, false);
}

struct ShapeAttrBData
{
   AtString attribute;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShapeAttrBData());
   AddMemUsage<ShapeAttrBData>();
}

node_update
{
   ShapeAttrBData *data = (ShapeAttrBData*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
}

node_finish
{
   ShapeAttrBData *data = (ShapeAttrBData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShapeAttrBData>();
}

shader_evaluate
{
   ShapeAttrBData *data = (ShapeAttrBData*) AiNodeGetLocalData(node);

   if (!AiUDataGetBool(data->attribute, sg->out.BOOL()))
   {
      sg->out.BOOL() = AiShaderEvalParamBool(p_default);
   }
}
