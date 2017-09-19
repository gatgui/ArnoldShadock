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
   AiParameterVec(SSTR::scale_pivot, 0.0f, 0.0f, 0.0f);
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
      data->S = AiM4Scaling(S);
   }

   data->Sp_set = false;
   if (!AiNodeIsLinked(node, SSTR::scale_pivot))
   {
      data->Sp_set = true;
      AtVector P = AiNodeGetVec(node, SSTR::scale_pivot);
      data->Sp = AiM4Translation(P);
      P = -P;
      data->iSp = AiM4Translation(P);
   }
   
   if (data->S_set && data->Sp_set)
   {
      AtMatrix tmp = AiM4Mult(data->S, data->iSp);
      data->Sf = AiM4Mult(data->Sp, tmp);
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
      S = AiM4Scaling(s);
   }
   else
   {
      if (data->Sp_set)
      {
         S = data->Sf;
         computeS = false;
      }
      else
      {
         S = data->S;
      }
   }
   
   if (!data->Sp_set)
   {
      p = AiShaderEvalParamVec(p_scale_pivot);
      ip = -p;
      
      P = AiM4Translation(p);
      iP = AiM4Translation(ip);
      
      tmp = AiM4Mult(S, iP);
      S = AiM4Mult(P, tmp);
   }
   else if (computeS)
   {
      tmp = AiM4Mult(S, data->iSp);
      S = AiM4Mult(data->Sp, tmp);
   }
   
   AtVector v = AiShaderEvalParamVec(p_input);
   
   sg->out.VEC() = AiM4VectorByMatrixMult(S, v);
}
