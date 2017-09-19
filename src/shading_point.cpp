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

AI_SHADER_NODE_EXPORT_METHODS(ShadingPointMtd);

enum ShadingPointParams
{
   p_time = 0,
   p_space,
   p_custom_space
};

enum ShadingPointSpace
{
   SPS_World = 0,
   SPS_Object,
   SPS_Custom
};

static const char* ShadingPointSpaceNames[] =
{
   "world",
   "object",
   "custom",
   NULL
};

node_parameters
{
   AtMatrix id = AiM4Identity();
   
   AiParameterFlt(SSTR::time, 0.0f);
   AiParameterEnum(SSTR::space, SPS_World, ShadingPointSpaceNames);
   AiParameterMtx(SSTR::custom_space, id);
}

struct ShadingPointData
{
   bool evalTime;
   float time;
   ShadingPointSpace space;
   bool evalCustomSpace;
   AtMatrix customSpace;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShadingPointData());
   AddMemUsage<ShadingPointData>();
}

node_update
{
   ShadingPointData *data = (ShadingPointData*) AiNodeGetLocalData(node);
   
   data->evalTime = AiNodeIsLinked(node, SSTR::time);
   
   if (!data->evalTime)
   {
      data->time = AiNodeGetFlt(node, SSTR::time);
   }
   
   data->space = (ShadingPointSpace) AiNodeGetInt(node, SSTR::space);
   
   data->evalCustomSpace = AiNodeIsLinked(node, SSTR::custom_space);
   
   if (data->space == SPS_Custom && !data->evalCustomSpace)
   {
      // custom space -> world matrix
      AtMatrix tmp;
      tmp = AiNodeGetMatrix(node, SSTR::custom_space);
      // world -> custom space matrix
      data->customSpace = AiM4Invert(tmp);
   }
}

node_finish
{
   ShadingPointData *data = (ShadingPointData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShadingPointData>();
}

shader_evaluate
{
   ShadingPointData *data = (ShadingPointData*) AiNodeGetLocalData(node);
   
   AtVector outP;
   
   AiShaderGlobalsGetPositionAtTime(sg, (data->evalTime ? AiShaderEvalParamFlt(p_time) : data->time), outP, 0, 0, 0);
   
   switch (data->space)
   {
   case SPS_Object:
      sg->out.VEC() = AiShaderGlobalsTransformPoint(sg, outP, AI_WORLD_TO_OBJECT);
      break;
      
   case SPS_Custom:
      {
         if (data->evalCustomSpace)
         {
            // custom space -> world matrix
            AtMatrix *tmp = AiShaderEvalParamMtx(p_custom_space);
            // world -> custom space matrix
            AtMatrix customSpace = AiM4Invert(*tmp);
            
            sg->out.VEC() = AiM4PointByMatrixMult(customSpace, outP);
         }
         else
         {
            sg->out.VEC() = AiM4PointByMatrixMult(data->customSpace, outP);
         }
      }
      break;
      
   case SPS_World:
   default:
      sg->out.VEC() = outP;
      break;
   }
}
