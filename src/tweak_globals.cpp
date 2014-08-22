#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TweakGlobalsMtd);

enum TweakGlobalsParams
{
   p_input = 0,
   p_P,
   p_P_space,
   p_P_world_matrix,
   p_N,
   p_N_space,
   p_N_world_matrix,
   p_fhemi,
   p_skip_shadow,
   p_traceset,
   p_inclusive_traceset

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
   AiParameterPnt("P", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("P_space", CS_World, CoordinateSpaceNames);
   AiParameterMtx("P_world_matrix", AI_M4_IDENTITY);
   AiParameterVec("N", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("N_space", CS_World, CoordinateSpaceNames);
   AiParameterMtx("N_world_matrix", AI_M4_IDENTITY);
   AiParameterBool("fhemi", true);
   AiParameterBool("skip_shadow", false);
   AiParameterStr("traceset", "");
   AiParameterBool("inclusive_traceset", true);
   
   AiMetaDataSetBool(mds, "P_space", "linkable", false);
   AiMetaDataSetBool(mds, "N_space", "linkable", false);
   AiMetaDataSetBool(mds, "fhemi", "linkable", false);
   AiMetaDataSetBool(mds, "skip_shadow", "linkable", false);
   AiMetaDataSetBool(mds, "traceset", "linkable", false);
   AiMetaDataSetBool(mds, "inclusive_traceset", "linkable", false);
}

struct TweakGlobalsData
{
   bool P_is_linked;
   CoordinateSpace P_space;
   bool P_world_matrix_is_linked;
   AtMatrix P_world_matrix;
   bool N_is_linked;
   CoordinateSpace N_space;
   bool N_world_matrix_is_linked;
   AtMatrix N_world_matrix;
   bool skip_shadow;
   const char *traceset;
   bool inclusive_traceset;
   bool fhemi;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(TweakGlobalsData)));
}

node_update
{
   TweakGlobalsData *data = (TweakGlobalsData*) AiNodeGetLocalData(node);
   
   data->P_is_linked = AiNodeIsLinked(node, "P");
   data->P_space = (CoordinateSpace) AiNodeGetInt(node, "P_space");
   data->P_world_matrix_is_linked = (data->P_space == CS_Custom && AiNodeIsLinked(node, "P_world_matrix"));
   if (!data->P_world_matrix_is_linked)
   {
      AiNodeGetMatrix(node, "P_world_matrix", data->P_world_matrix);
   }
   
   data->N_is_linked = AiNodeIsLinked(node, "N");
   data->N_space = (CoordinateSpace) AiNodeGetInt(node, "N_space");
   data->N_world_matrix_is_linked = (data->N_space == CS_Custom && AiNodeIsLinked(node, "N_world_matrix"));
   if (!data->N_world_matrix_is_linked)
   {
      AiNodeGetMatrix(node, "N_world_matrix", data->N_world_matrix);
   }
   
   data->fhemi = AiNodeGetBool(node, "fhemi");
   data->skip_shadow = AiNodeGetBool(node, "skip_shadow");
   data->traceset = AiNodeGetStr(node, "traceset");
   data->inclusive_traceset = AiNodeGetBool(node, "inclusive_traceset");
   
   if (data->traceset && data->traceset[0] == '\0')
   {
      data->traceset = 0;
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   TweakGlobalsData *data = (TweakGlobalsData*) AiNodeGetLocalData(node);
   
   AtPoint old_P = sg->P;
   AtPoint old_Po = sg->Po;
   AtVector old_N = sg->N;
   AtVector old_Nf = sg->Nf;
   AtVector old_Ns = sg->Ns;
   const char *old_traceset = sg->traceset;
   bool old_inclusive_traceset = sg->inclusive_traceset;
   bool old_skip_shadow = sg->skip_shadow;
   bool old_fhemi = sg->fhemi;
   
   if (data->P_is_linked)
   {
      if (data->P_space == CS_World)
      {
         sg->P = AiShaderEvalParamPnt(p_P);
         AiM4PointByMatrixMult(&(sg->Po), sg->Minv, &(sg->P));
      }
      else if (data->P_space == CS_Object)
      {
         sg->Po = AiShaderEvalParamPnt(p_P);
         AiM4PointByMatrixMult(&(sg->P), sg->M, &(sg->Po));
      }
      else
      {
         AtPoint P = AiShaderEvalParamPnt(p_P);
         if (!data->P_world_matrix_is_linked)
         {
            AtMatrix *W = AiShaderEvalParamMtx(p_P_world_matrix);
            AiM4PointByMatrixMult(&(sg->P), *W, &P);
         }
         else
         {
            AiM4PointByMatrixMult(&(sg->P), data->P_world_matrix, &P);
         }
         AiM4PointByMatrixMult(&(sg->Po), sg->Minv, &(sg->P));
      }
   }
   
   if (data->N_is_linked)
   {
      AtVector N = AiV3Normalize(AiShaderEvalParamVec(p_N));
      
      if (data->N_space == CS_Object)
      {
         AiM4VectorByMatrixMult(&(sg->N), sg->M, &N);
         sg->N = AiV3Normalize(sg->N);
      }
      else if (data->N_space == CS_Custom)
      {
         if (!data->N_world_matrix_is_linked)
         {
            AtMatrix *W = AiShaderEvalParamMtx(p_N_world_matrix);
            AiM4VectorByMatrixMult(&(sg->N), *W, &N);
         }
         else
         {
            AiM4VectorByMatrixMult(&(sg->N), data->N_world_matrix, &N);
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
   
   if (data->traceset)
   {
      AiShaderGlobalsSetTraceSet(sg, data->traceset, data->inclusive_traceset);
   }
   
   sg->skip_shadow = data->skip_shadow;
   sg->fhemi = data->fhemi;
   
   sg->out.RGB = AiShaderEvalParamRGB(p_input);
   
   sg->fhemi = old_fhemi;
   sg->skip_shadow = old_skip_shadow;
   
   if (data->traceset)
   {
      if (!old_traceset || old_traceset[0] == '\0')
      {
         AiShaderGlobalsUnsetTraceSet(sg);
      }
      else
      {
         AiShaderGlobalsSetTraceSet(sg, old_traceset, old_inclusive_traceset);
      }
   }
   
   if (data->N_is_linked)
   {
      sg->N = old_N;
      sg->Ns = old_Ns;
      sg->Nf = old_Nf;
   }
   
   if (data->P_is_linked)
   {
      sg->P = old_P;
      sg->Po = old_Po;
   }
}


