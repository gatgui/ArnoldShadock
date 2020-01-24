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

AI_SHADER_NODE_EXPORT_METHODS(TweakGlobalsMtd);

enum TweakGlobalsParams
{
   p_input = 0,
   p_P,
   p_P_space,
   p_P_world_matrix,
   p_P_is_offset,
   p_N,
   p_N_space,
   p_N_world_matrix,
   p_fhemi,
   p_skip_shadow,
   p_trace_set,
   p_inclusive_trace_set

};

enum CoordinateSpace
{
   CS_World = 0,
   CS_Object,
   CS_Custom
};

static const char* CoordinateSpaceNames[] =
{
   "world",
   "object",
   "custom",
   NULL
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::P, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::P_space, CS_World, CoordinateSpaceNames);
   AiParameterMtx(SSTR::P_world_matrix, AI_M4_IDENTITY);
   AiParameterBool(SSTR::P_is_offset, false);
   AiParameterVec(SSTR::N, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::N_space, CS_World, CoordinateSpaceNames);
   AiParameterMtx(SSTR::N_world_matrix, AI_M4_IDENTITY);
   AiParameterBool(SSTR::fhemi, true);
   AiParameterBool(SSTR::skip_shadow, false);
   AiParameterStr(SSTR::trace_set, "");
   AiParameterBool(SSTR::inclusive_trace_set, true);
}

struct TweakGlobalsData
{
   bool evalP;
   CoordinateSpace PSpace;
   bool evalPWorldMatrix;
   AtMatrix PWorldMatrix;
   bool PIsOffset;
   bool evalN;
   CoordinateSpace NSpace;
   bool evalNWorldMatrix;
   AtMatrix NWorldMatrix;
   bool skipShadow;
   AtString traceset;
   bool inclusiveTraceset;
   bool fhemi;
};

node_initialize
{
   AiNodeSetLocalData(node, new TweakGlobalsData());
   AddMemUsage<TweakGlobalsData>();
}

node_update
{
   TweakGlobalsData *data = (TweakGlobalsData*) AiNodeGetLocalData(node);
   
   data->evalP = AiNodeIsLinked(node, SSTR::P);
   data->PSpace = (CoordinateSpace) AiNodeGetInt(node, SSTR::P_space);
   data->PIsOffset = AiNodeGetBool(node, SSTR::P_is_offset);
   data->evalPWorldMatrix = (data->PSpace == CS_Custom && AiNodeIsLinked(node, SSTR::P_world_matrix));
   if (!data->evalPWorldMatrix)
   {
      data->PWorldMatrix = AiNodeGetMatrix(node, SSTR::P_world_matrix);
   }
   
   data->evalN = AiNodeIsLinked(node, SSTR::N);
   data->NSpace = (CoordinateSpace) AiNodeGetInt(node, SSTR::N_space);
   data->evalNWorldMatrix = (data->NSpace == CS_Custom && AiNodeIsLinked(node, SSTR::N_world_matrix));
   if (!data->evalNWorldMatrix)
   {
      data->NWorldMatrix = AiNodeGetMatrix(node, SSTR::N_world_matrix);
   }
   
   data->fhemi = AiNodeGetBool(node, SSTR::fhemi);
   data->skipShadow = AiNodeGetBool(node, SSTR::skip_shadow);
   data->traceset = AiNodeGetStr(node, SSTR::trace_set);
   data->inclusiveTraceset = AiNodeGetBool(node, SSTR::inclusive_trace_set);
}

node_finish
{
   TweakGlobalsData *data = (TweakGlobalsData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<TweakGlobalsData>();
}

shader_evaluate
{
   TweakGlobalsData *data = (TweakGlobalsData*) AiNodeGetLocalData(node);
   
   AtVector oldP = sg->P;
   AtVector oldPo = sg->Po;
   AtVector oldN = sg->N;
   AtVector oldNf = sg->Nf;
   AtVector oldNs = sg->Ns;
   AtString oldTraceset = sg->traceset;
   bool oldInclusiveTraceset = sg->inclusive_traceset;
   bool oldSkipShadow = sg->skip_shadow;
   bool oldFhemi = sg->fhemi;
   
   if (data->evalP)
   {
      if (data->PSpace == CS_World)
      {
         sg->P = (data->PIsOffset ? sg->P : AI_P3_ZERO) + AiShaderEvalParamVec(p_P);
         sg->Po = AiM4PointByMatrixMult(sg->Minv, sg->P);
      }
      else if (data->PSpace == CS_Object)
      {
         sg->Po = (data->PIsOffset ? sg->Po : AI_P3_ZERO) + AiShaderEvalParamVec(p_P);
         sg->P = AiM4PointByMatrixMult(sg->M, sg->Po);
      }
      else
      {
         AtVector P = AiShaderEvalParamVec(p_P);
         AtVector Pw = AI_P3_ZERO;
         if (!data->evalPWorldMatrix)
         {
            AtMatrix *W = AiShaderEvalParamMtx(p_P_world_matrix);
            Pw = AiM4PointByMatrixMult(*W, P);
         }
         else
         {
            Pw = AiM4PointByMatrixMult(data->PWorldMatrix, P);
         }
         if (data->PIsOffset)
         {
            Pw += sg->P;
         }
         sg->P = Pw;
         sg->Po = AiM4PointByMatrixMult(sg->Minv, sg->P);
      }
   }
   
   if (data->evalN)
   {
      AtVector N = AiV3Normalize(AiShaderEvalParamVec(p_N));
      
      if (data->NSpace == CS_Object)
      {
         sg->N = AiM4VectorByMatrixMult(sg->M, N);
         sg->N = AiV3Normalize(sg->N);
      }
      else if (data->NSpace == CS_Custom)
      {
         if (!data->evalNWorldMatrix)
         {
            AtMatrix *W = AiShaderEvalParamMtx(p_N_world_matrix);
            sg->N = AiM4VectorByMatrixMult(*W, N);
         }
         else
         {
            sg->N = AiM4VectorByMatrixMult(data->NWorldMatrix, N);
         }
         sg->N = AiV3Normalize(sg->N);
      }
      else
      {
         sg->N = N;
      }
      
      sg->Ns = sg->N;
      
      sg->Nf = sg->N;
      AiFaceForward(sg->Nf, sg->Rd);
   }
   
   if (!data->traceset.empty())
   {
      AiShaderGlobalsSetTraceSet(sg, data->traceset, data->inclusiveTraceset);
   }
   
   sg->skip_shadow = data->skipShadow;
   sg->fhemi = data->fhemi;
   
   sg->out.RGB() = AiShaderEvalParamRGB(p_input);
   
   sg->fhemi = oldFhemi;
   sg->skip_shadow = oldSkipShadow;
   
   if (!data->traceset.empty())
   {
      if (oldTraceset.empty())
      {
         AiShaderGlobalsUnsetTraceSet(sg);
      }
      else
      {
         AiShaderGlobalsSetTraceSet(sg, oldTraceset, oldInclusiveTraceset);
      }
   }
   
   if (data->evalN)
   {
      sg->N = oldN;
      sg->Ns = oldNs;
      sg->Nf = oldNf;
   }
   
   if (data->evalP)
   {
      sg->P = oldP;
      sg->Po = oldPo;
   }
}


