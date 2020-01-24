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

AI_SHADER_NODE_EXPORT_METHODS(C3ToVMtd);

enum C3ToVParams
{
   p_input = 0,
   p_mode
};

enum ColorToVectorMode
{
   CTV_RAW = 0,
   CTV_HSV,
   CTV_HSL
};

static const char* ColorToVectorModeNames[] =
{
   "raw",
   "hsv",
   "hsl",
   NULL
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::mode, CTV_RAW, ColorToVectorModeNames);
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
   static float sNormalizeHue = 60.0f / 360.0f;
   
   AtRGB input = AiShaderEvalParamRGB(p_input);
   ColorToVectorMode mode = (ColorToVectorMode) *((int*) AiNodeGetLocalData(node));
   
   switch (mode)
   {
   case CTV_HSV:
      {
         float M = AiMax(input.r, input.g, input.b);
         float m = AiMin(input.r, input.g, input.b);
         float C = M - m;
         
         float hue = 0.0f;
         float sat = 0.0f;
         float val = 0.0f;
         
         val = M;
         
         if (M >= AI_EPSILON)
         {
            sat = C / M;
         }
         
         if (sat >= AI_EPSILON)
         {
            if (M == input.r)
            {
               hue = (input.g - input.b) / C;
            }
            else if (M == input.g)
            {
               hue = (input.b - input.r) / C + 2.0f;
            }
            else
            {
               hue = (input.r - input.g) / C + 4.0f;
            }
            
            hue *= sNormalizeHue;
            
            if (hue < 0.0f)
            {
               hue += 1.0f;
            }
         }
         
         sg->out.VEC().x = hue;
         sg->out.VEC().y = sat;
         sg->out.VEC().z = val;
      }
      break;
   case CTV_HSL:
      {
         float M = AiMax(input.r, input.g, input.b);
         float m = AiMin(input.r, input.g, input.b);
         float C = M - m;
         
         float hue = 0.0f;
         float sat = 0.0f;
         float lum = 0.0f;
         
         lum = 0.5f * (m + M);
         
         if (C >= AI_EPSILON)
         {
            if (lum <= 0.5f)
            {
               sat = C / (2.0f * lum);
            }
            else
            {
               sat = C / (2.0f * (1.0f - lum));
            }
            
            if (M == input.r)
            {
               hue = (input.g - input.b) / C;
            }
            else if (M == input.g)
            {
               hue = (input.b - input.r) / C + 2.0f;
            }
            else
            {
               hue = (input.r - input.g) / C + 4.0f;
            }
            
            hue *= sNormalizeHue;
            
            if (hue < 0.0f)
            {
               hue += 1.0f;
            }
         }
         
         sg->out.VEC().x = hue;
         sg->out.VEC().y = sat;
         sg->out.VEC().z = lum;
      }
      break;
   case CTV_RAW:
   default:
      sg->out.VEC().x = input.r;
      sg->out.VEC().y = input.g;
      sg->out.VEC().z = input.b;
   }
}
