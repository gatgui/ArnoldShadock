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
   AiParameterPnt(SSTR::P, 0.0f, 0.0f, 0.0f);
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
   
   AiMetaDataSetBool(mds, SSTR::P_space, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::P_is_offset, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::N_space, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::fhemi, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::skip_shadow, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::trace_set, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::inclusive_trace_set, SSTR::linkable, false);
}

struct NodeData
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
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->evalP = AiNodeIsLinked(node, SSTR::P);
   data->PSpace = (CoordinateSpace) AiNodeGetInt(node, SSTR::P_space);
   data->PIsOffset = AiNodeGetBool(node, SSTR::P_is_offset);
   data->evalPWorldMatrix = (data->PSpace == CS_Custom && AiNodeIsLinked(node, SSTR::P_world_matrix));
   if (!data->evalPWorldMatrix)
   {
      AiNodeGetMatrix(node, SSTR::P_world_matrix, data->PWorldMatrix);
   }
   
   data->evalN = AiNodeIsLinked(node, SSTR::N);
   data->NSpace = (CoordinateSpace) AiNodeGetInt(node, SSTR::N_space);
   data->evalNWorldMatrix = (data->NSpace == CS_Custom && AiNodeIsLinked(node, SSTR::N_world_matrix));
   if (!data->evalNWorldMatrix)
   {
      AiNodeGetMatrix(node, SSTR::N_world_matrix, data->NWorldMatrix);
   }
   
   data->fhemi = AiNodeGetBool(node, SSTR::fhemi);
   data->skipShadow = AiNodeGetBool(node, SSTR::skip_shadow);
   data->traceset = AiNodeGetStr(node, SSTR::trace_set);
   data->inclusiveTraceset = AiNodeGetBool(node, SSTR::inclusive_trace_set);
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   AtPoint oldP = sg->P;
   AtPoint oldPo = sg->Po;
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
         sg->P = (data->PIsOffset ? sg->P : AI_P3_ZERO) + AiShaderEvalParamPnt(p_P);
         AiM4PointByMatrixMult(&(sg->Po), sg->Minv, &(sg->P));
      }
      else if (data->PSpace == CS_Object)
      {
         sg->Po = (data->PIsOffset ? sg->Po : AI_P3_ZERO) + AiShaderEvalParamPnt(p_P);
         AiM4PointByMatrixMult(&(sg->P), sg->M, &(sg->Po));
      }
      else
      {
         AtPoint P = AiShaderEvalParamPnt(p_P);
         AtPoint Pw = AI_P3_ZERO;
         if (!data->evalPWorldMatrix)
         {
            AtMatrix *W = AiShaderEvalParamMtx(p_P_world_matrix);
            AiM4PointByMatrixMult(&Pw, *W, &P);
         }
         else
         {
            AiM4PointByMatrixMult(&Pw, data->PWorldMatrix, &P);
         }
         if (data->PIsOffset)
         {
            Pw += sg->P;
         }
         sg->P = Pw;
         AiM4PointByMatrixMult(&(sg->Po), sg->Minv, &(sg->P));
      }
   }
   
   if (data->evalN)
   {
      AtVector N = AiV3Normalize(AiShaderEvalParamVec(p_N));
      
      if (data->NSpace == CS_Object)
      {
         AiM4VectorByMatrixMult(&(sg->N), sg->M, &N);
         sg->N = AiV3Normalize(sg->N);
      }
      else if (data->NSpace == CS_Custom)
      {
         if (!data->evalNWorldMatrix)
         {
            AtMatrix *W = AiShaderEvalParamMtx(p_N_world_matrix);
            AiM4VectorByMatrixMult(&(sg->N), *W, &N);
         }
         else
         {
            AiM4VectorByMatrixMult(&(sg->N), data->NWorldMatrix, &N);
         }
         sg->N = AiV3Normalize(sg->N);
      }
      else
      {
         sg->N = N;
      }
      
      sg->Ns = sg->N;
      
      sg->Nf = sg->N;
      AiFaceForward(&(sg->Nf), sg->Rd);
   }
   
   if (!data->traceset.empty())
   {
      AiShaderGlobalsSetTraceSet(sg, data->traceset, data->inclusiveTraceset);
   }
   
   sg->skip_shadow = data->skipShadow;
   sg->fhemi = data->fhemi;
   
   sg->out.RGB = AiShaderEvalParamRGB(p_input);
   
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


