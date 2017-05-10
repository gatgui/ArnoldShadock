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

AI_SHADER_NODE_EXPORT_METHODS(MakeFrameMtd);

enum MakeFrameParams
{
   p_method = 0,
   p_origin,
   p_custom_origin,
   p_z_axis,
   p_custom_axis
};

enum Origin
{
   O_P,
   O_custom
};

enum ZAxis
{
   ZA_N,
   ZA_Nf,
   ZA_custom
};

enum Method
{
   M_polar = 0,
   M_shirley,
   M_x_along_dPdu,
   M_x_along_dPdv
};

static const char* OriginNames[] = { "P", "custom", NULL };

static const char* ZAxisNames[] = { "N", "Nf", "custom", NULL };

static const char* MethodNames[] = { "polar", "shirley", "x_along_dPdu", "x_along_dPdv", NULL };

node_parameters
{
   AiParameterEnum(SSTR::method, M_x_along_dPdu, MethodNames);
   AiParameterEnum(SSTR::origin, O_P, OriginNames);
   AiParameterPnt(SSTR::custom_origin, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::z_axis, ZA_Nf, ZAxisNames);
   AiParameterVec(SSTR::custom_axis, 0.0f, 0.0f, 0.0f);
}

struct MakeFrameData
{
   Method method;
   Origin origin;
   bool evalCustomOrigin;
   AtVector customOrigin;
   ZAxis zAxis;
   bool evalCustomAxis;
   AtVector customAxis;
};

node_initialize
{
   AiNodeSetLocalData(node, new MakeFrameData());
   AddMemUsage<MakeFrameData>();
}

node_update
{
   MakeFrameData *data = (MakeFrameData*) AiNodeGetLocalData(node);
   
   data->method = (Method) AiNodeGetInt(node, SSTR::method);
   data->origin = (Origin) AiNodeGetInt(node, SSTR::origin);
   data->evalCustomOrigin = AiNodeIsLinked(node, SSTR::custom_origin);
   data->zAxis = (ZAxis) AiNodeGetInt(node, SSTR::z_axis);
   data->evalCustomAxis = AiNodeIsLinked(node, SSTR::custom_axis);
   
   if (data->origin == O_custom && !data->evalCustomOrigin)
   {
      data->customOrigin = AiNodeGetPnt(node, SSTR::custom_origin);
   }
   
   if (data->zAxis == ZA_custom && !data->evalCustomAxis)
   {
      data->customAxis = AiV3Normalize(AiNodeGetVec(node, SSTR::custom_axis));
   }
}

node_finish
{
   MakeFrameData *data = (MakeFrameData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<MakeFrameData>();
}

shader_evaluate
{
   MakeFrameData *data = (MakeFrameData*) AiNodeGetLocalData(node);
   
   AtVector O, X, Y, Z;
   
   // Read origin
   if (data->origin == O_P)
   {
      O = sg->P;
   }
   else if (!data->evalCustomOrigin)
   {
      O = data->customOrigin;
   }
   else
   {
      O = AiShaderEvalParamPnt(p_custom_origin);
   }
   
   // Read Z axis
   if (data->zAxis == ZA_N)
   {
      Z = sg->N;
   }
   else if (data->zAxis == ZA_Nf)
   {
      Z = sg->Nf;
   }
   else if (!data->evalCustomAxis)
   {
      Z = data->customAxis;
   }
   else
   {
      Z = AiV3Normalize(AiShaderEvalParamVec(p_custom_axis));
   }
   
   // Build X and Y axis
   switch (data->method)
   {
   case M_x_along_dPdu:
      if (!AiV3IsZero(sg->dPdu))
      {
         X = AiV3Normalize(sg->dPdu);
         Y = AiV3Normalize(AiV3Cross(Z, X));
         X = AiV3Cross(Y, Z);
      }
      else
      {
         AiBuildLocalFramePolar(&X, &Y, &Z);
      }
      break;
   case M_x_along_dPdv:
      if (!AiV3IsZero(sg->dPdv))
      {
         X = AiV3Normalize(sg->dPdv);
         Y = AiV3Normalize(AiV3Cross(Z, X));
         X = AiV3Cross(Y, Z);
      }
      else
      {
         AiBuildLocalFramePolar(&X, &Y, &Z);
      }
      break;
   case M_shirley:
      AiBuildLocalFrameShirley(&X, &Y, &Z);
      break;
   case M_polar:
   default:
      AiBuildLocalFramePolar(&X, &Y, &Z);
      break;
   }
   
   sg->out.pMTX = (AtMatrix*) AiShaderGlobalsQuickAlloc(sg, sizeof(AtMatrix));
   
   AiM4Frame(*(sg->out.pMTX), &O, &X, &Y, &Z);
}
