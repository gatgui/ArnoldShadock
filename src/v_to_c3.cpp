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

AI_SHADER_NODE_EXPORT_METHODS(VToC3Mtd);

enum VToC3Params
{
   p_input = 0,
   p_mode
};

enum VectorToColorMode
{
   VTC_RAW = 0,
   VTC_HSV,
   VTC_HSL
};

static const char* VectorToColorModeNames[] =
{
   "raw",
   "hsv",
   "hsl",
   NULL
};


node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::mode, VTC_RAW, VectorToColorModeNames);
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
   AtVector input = AiShaderEvalParamVec(p_input);
   
   VectorToColorMode mode = (VectorToColorMode) *((int*) AiNodeGetLocalData(node));
   
   switch (mode)
   {
   case VTC_HSV:
      {
         float C = input.z * input.y;
         float h = input.x * 6.0f;
         float X = C * (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));
         
         if (h < 1.0f)
         {
            sg->out.RGB().r = C;
            sg->out.RGB().g = X;
            sg->out.RGB().b = 0.0f;
         }
         else if (h < 2.0f)
         {
            sg->out.RGB().r = X;
            sg->out.RGB().g = C;
            sg->out.RGB().b = 0.0f;
         }
         else if (h < 3.0f)
         {
            sg->out.RGB().r = 0.0f;
            sg->out.RGB().g = C;
            sg->out.RGB().b = X;
         }
         else if (h < 4.0f)
         {
            sg->out.RGB().r = 0.0f;
            sg->out.RGB().g = X;
            sg->out.RGB().b = C;
         }
         else if (h < 5.0f)
         {
            sg->out.RGB().r = X;
            sg->out.RGB().g = 0.0f;
            sg->out.RGB().b = C;
         }
         else
         {
            sg->out.RGB().r = C;
            sg->out.RGB().g = 0.0f;
            sg->out.RGB().b = X;
         }
         
         float m = input.z - C;
         
         sg->out.RGB().r += m;
         sg->out.RGB().g += m;
         sg->out.RGB().b += m;
      }
      break;
   case VTC_HSL:
      {
         float C;
         
         if (input.z <= 0.5f)
         {
            C = 2.0f * input.z * input.y;
         }
         else
         {
            C = (2.0f - 2.0f * input.z) * input.y;
         }
         
         float h = input.x * 6.0f;
         float X = C * (1.0f - fabsf(fmodf(h, 2.0f) - 1));
         
         if (h < 1.0f)
         {
            sg->out.RGB().r = C;
            sg->out.RGB().g = X;
            sg->out.RGB().b = 0.0f;
         }
         else if (h < 2.0f)
         {
            sg->out.RGB().r = X;
            sg->out.RGB().g = C;
            sg->out.RGB().b = 0.0f;
         }
         else if (h < 3.0f)
         {
            sg->out.RGB().r = 0.0f;
            sg->out.RGB().g = C;
            sg->out.RGB().b = X;
         }
         else if (h < 4.0f)
         {
            sg->out.RGB().r = 0.0f;
            sg->out.RGB().g = X;
            sg->out.RGB().b = C;
         }
         else if (h < 5.0f)
         {
            sg->out.RGB().r = X;
            sg->out.RGB().g = 0.0f;
            sg->out.RGB().b = C;
         }
         else
         {
            sg->out.RGB().r = C;
            sg->out.RGB().g = 0.0f;
            sg->out.RGB().b = X;
         }
         
         float m = input.z - 0.5f * C;
         
         sg->out.RGB().r += m;
         sg->out.RGB().g += m;
         sg->out.RGB().b += m;
      }
      break;
   case VTC_RAW:
   default:
      sg->out.RGB().r = input.x;
      sg->out.RGB().g = input.y;
      sg->out.RGB().b = input.z;
   }
}
