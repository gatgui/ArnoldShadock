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

AI_SHADER_NODE_EXPORT_METHODS(FToIMtd);

enum FToIParams
{
   p_input = 0,
   p_mode
};

static const char* FloatToIntModeNames[] =
{
   "floor",
   "ceil",
   "round",
   "truncate",
   NULL
};

enum FloatToIntMode
{
   FTI_FLOOR = 0,
   FTI_CEIL,
   FTI_ROUND,
   FTI_TRUNCATE
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterEnum(SSTR::mode, 0, FloatToIntModeNames);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *data = (int*) AiNodeGetLocalData(node);
   *data = AiNodeGetInt(node, SSTR::mode);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

shader_evaluate
{
   FloatToIntMode mode = (FloatToIntMode) *((int*) AiNodeGetLocalData(node));
   float input = AiShaderEvalParamFlt(p_input);
   
   switch (mode)
   {
   case FTI_CEIL:
      sg->out.INT() = static_cast<int>(ceilf(input));
      return;
   case FTI_ROUND:
      sg->out.INT() = static_cast<int>(floorf(0.5f + input));
      return;
   case FTI_TRUNCATE:
      sg->out.INT() = static_cast<int>(input < 0.0f ? -floorf(-input) : floorf(input));
      return;
   case FTI_FLOOR:
   default:
      sg->out.INT() = static_cast<int>(floorf(input));
      return;
   }
}
