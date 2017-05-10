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

AI_SHADER_NODE_EXPORT_METHODS(ScaleMtd);

enum ScaleParams
{
   p_input = 0,
   p_scale_pivot,
   p_scale
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::scale, 1.0f, 1.0f, 1.0f);
   AiParameterPnt(SSTR::scale_pivot, 0.0f, 0.0f, 0.0f);
}

struct ScaleData
{
   AtMatrix S;
   AtMatrix Sp;
   AtMatrix iSp;
   AtMatrix Sf; // final scale matrix -> iSp * S * Sp

   bool S_set;
   bool Sp_set;
};

node_initialize
{
   ScaleData *data = new ScaleData();
   AddMemUsage<ScaleData>();

   data->S_set = false;
   data->Sp_set = false;

   AiNodeSetLocalData(node, data);
}

node_update
{
   ScaleData *data = (ScaleData*) AiNodeGetLocalData(node);
   
   data->S_set = false;
   if (!AiNodeIsLinked(node, SSTR::scale))
   {
      data->S_set = true;
      AtVector S = AiNodeGetVec(node, SSTR::scale);
      AiM4Scaling(data->S, &S);
   }

   data->Sp_set = false;
   if (!AiNodeIsLinked(node, SSTR::scale_pivot))
   {
      data->Sp_set = true;
      AtPoint P = AiNodeGetPnt(node, SSTR::scale_pivot);
      AiM4Translation(data->Sp, &P);
      P = -P;
      AiM4Translation(data->iSp, &P);
   }
   
   if (data->S_set && data->Sp_set)
   {
      AtMatrix tmp;
      AiM4Mult(tmp, data->S, data->iSp);
      AiM4Mult(data->Sf, data->Sp, tmp);
   }
}

node_finish
{
   ScaleData *data = (ScaleData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ScaleData>();
}

shader_evaluate
{
   ScaleData *data = (ScaleData*) AiNodeGetLocalData(node);
   
   AtVector p, ip, s;
   AtMatrix S, P, iP, tmp;
   bool computeS = true;
   
   if (!data->S_set)
   {
      s = AiShaderEvalParamVec(p_scale);
      AiM4Scaling(S, &s);
   }
   else
   {
      if (data->Sp_set)
      {
         AiM4Copy(S, data->Sf);
         computeS = false;
      }
      else
      {
         AiM4Copy(S, data->S);
      }
   }
   
   if (!data->Sp_set)
   {
      p = AiShaderEvalParamVec(p_scale_pivot);
      ip = -p;
      
      AiM4Translation(P, &p);
      AiM4Translation(iP, &ip);
      
      AiM4Mult(tmp, S, iP);
      AiM4Mult(S, P, tmp);
   }
   else if (computeS)
   {
      AiM4Mult(tmp, S, data->iSp);
      AiM4Mult(S, data->Sp, tmp);
   }
   
   AtVector v = AiShaderEvalParamVec(p_input);
   
   AiM4VectorByMatrixMult(&(sg->out.VEC), S, &v);
}
