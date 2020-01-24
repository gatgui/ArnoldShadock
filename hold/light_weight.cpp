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

AI_SHADER_NODE_EXPORT_METHODS(LightWeightMtd);

enum LightWeightParams
{
   p_weight = 0
};

enum LightWeight
{
   LW_diffuse = 0,
   LW_specular,
   LW_sss
};

static const char* LightWeightNames[] =
{
   "diffuse",
   "specular",
   "sss",
   NULL
};

node_parameters
{
   AiParameterEnum(SSTR::weight, LW_diffuse, LightWeightNames);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *pw = (int*) AiNodeGetLocalData(node);
   *pw = AiNodeGetInt(node, SSTR::weight);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

shader_evaluate
{
   int weight = *((int*) AiNodeGetLocalData(node));
   AtLightSample ls;
   AiLightsGetSample(sg, ls);
   
   if (ls.Lp)
   {
      switch (weight)
      {
      case LW_diffuse:
         sg->out.FLT() = AiLightGetDiffuse(ls.Lp);
         return;
      
      case LW_specular:
         sg->out.FLT() = AiLightGetSpecular(ls.Lp);
         return;
      
      case LW_sss:
         sg->out.FLT() = AiLightGetSSS(ls.Lp);
         return;
      
      default:
         break;
      }
   }
   
   sg->out.FLT() = 0.0f;
}
