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

AI_SHADER_NODE_EXPORT_METHODS(ShadingNormalMtd);

enum ShadingNormalParams
{
   p_time = 0,
   p_mode,
   p_faceforward,
   p_space,
   p_custom_space
};

enum ShadingNormalMode
{
   SNM_Default = 0,
   SNM_Geometric,
   SNM_Smoothed
};

enum ShadingNormalSpace
{
   SNS_World = 0,
   SNS_Object,
   SNS_Custom
};

static const char* ShadingNormalModeNames[] =
{
   "default",
   "geometric",
   "smoothed",
   NULL
};

static const char* ShadingNormalSpaceNames[] =
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
   AiParameterEnum(SSTR::mode, SNM_Default, ShadingNormalModeNames);
   AiParameterBool(SSTR::faceforward, false);
   AiParameterEnum(SSTR::space, SNS_World, ShadingNormalSpaceNames);
   AiParameterMtx(SSTR::custom_space, id);
}

struct ShadingNormalData
{
   bool evalTime;
   float time;
   ShadingNormalMode mode;
   bool faceforward;
   ShadingNormalSpace space;
   bool evalCustomSpace;
   AtMatrix customSpace;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShadingNormalData());
   AddMemUsage<ShadingNormalData>();
}

node_update
{
   ShadingNormalData *data = (ShadingNormalData*) AiNodeGetLocalData(node);
   
   data->evalTime = AiNodeIsLinked(node, SSTR::time);
   
   if (!data->evalTime)
   {
      data->time = AiNodeGetFlt(node, SSTR::time);
   }
   
   data->mode = (ShadingNormalMode) AiNodeGetInt(node, SSTR::mode);
   
   data->faceforward = AiNodeGetBool(node, SSTR::faceforward);
   
   data->space = (ShadingNormalSpace) AiNodeGetInt(node, SSTR::space);
   
   data->evalCustomSpace = AiNodeIsLinked(node, SSTR::custom_space);
   
   if (data->space == SNS_Custom && !data->evalCustomSpace)
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
   ShadingNormalData *data = (ShadingNormalData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShadingNormalData>();
}

shader_evaluate
{
   ShadingNormalData *data = (ShadingNormalData*) AiNodeGetLocalData(node);
   
   AtVector P, N, Ns, Ng, outN;
   
   AiShaderGlobalsGetPositionAtTime(sg, (data->evalTime ? AiShaderEvalParamFlt(p_time) : data->time), P, &N, &Ng, &Ns);
   
   switch (data->mode)
   {
   case SNM_Geometric:
      outN = Ng;
      break;
      
   case SNM_Smoothed:
      outN = Ns;
      break;
      
   default:
      outN = N;
   }
   
   if (data->faceforward)
   {
      AiFaceForward(outN, sg->Rd);
   }
   
   switch (data->space)
   {
   case SNS_Object:
      sg->out.VEC() = AiShaderGlobalsTransformNormal(sg, outN, AI_WORLD_TO_OBJECT);
      break;
      
   case SNS_Custom:
      {
         AtVector csN;
         
         if (data->evalCustomSpace)
         {
            // custom space -> world matrix 
            AtMatrix *tmp = AiShaderEvalParamMtx(p_custom_space);
            // world -> custom space matrix
            AtMatrix customSpace;
            customSpace = AiM4Invert(*tmp);
            
            csN = AiM4VectorByMatrixMult(customSpace, outN);
         }
         else
         {
            csN = AiM4VectorByMatrixMult(data->customSpace, outN);
         }
         
         sg->out.VEC() = AiV3Normalize(csN);
      }
      break;
      
   case SNS_World:
   default:
      sg->out.VEC() = outN;
      break;
   }
}
