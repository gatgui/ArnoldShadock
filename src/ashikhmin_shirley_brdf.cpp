#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(AshikhminShirleyBrdfMtd);

enum AshikhminShirleyBrdfParams
{
   p_glossiness_x = 0,
   p_glossiness_y,
   p_N,
   p_N_space,
   p_u,
   p_u_space
};

node_parameters
{
   AiParameterFlt("glossiness_x", 0.467f);
   AiParameterFlt("glossiness_y", 0.467f);
   AiParameterVec("N", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("N_space", CS_World, CoordinateSpaceNames);
   AiParameterVec("u", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("u_space", CS_World, CoordinateSpaceNames);
   
   AiMetaDataSetBool(mds, "u_space", "linkable", false);
   AiMetaDataSetBool(mds, "N_space", "linkable", false);
}

struct AshikhminShirleyBrdfData
{
   bool u_is_connected;
   CoordinateSpace u_space;
   bool N_is_connected;
   CoordinateSpace N_space;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(AshikhminShirleyBrdfData)));
}

node_update
{
   AshikhminShirleyBrdfData *data = (AshikhminShirleyBrdfData*) AiNodeGetLocalData(node);
   
   data->u_is_connected = AiNodeIsLinked(node, "u");
   data->u_space = (CoordinateSpace) AiNodeGetInt(node, "u_space");
   data->N_is_connected = AiNodeIsLinked(node, "N");
   data->N_space = (CoordinateSpace) AiNodeGetInt(node, "N_space");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   AshikhminShirleyBrdfData *node_data = (AshikhminShirleyBrdfData*) AiNodeGetLocalData(node);
   BRDFData *brdf_data = (BRDFData*) AiShaderGlobalsQuickAlloc(sg, sizeof(BRDFData));
   
   float gx = AiShaderEvalParamFlt(p_glossiness_x);
   float gy = AiShaderEvalParamFlt(p_glossiness_y);
   AtVector N, u, v;
   bool computeV = true;
   
   if (node_data->N_is_connected)
   {
      N = AiV3Normalize(AiShaderEvalParamVec(p_N));
      if (node_data->N_space == CS_Object)
      {
         AtVector No = N;
         AiM4VectorByMatrixMult(&N, sg->M, &No);
         N = AiV3Normalize(N);
      }
   }
   else
   {
      N = sg->Nf;
   }
   
   if (node_data->u_is_connected)
   {
      u = AiV3Normalize(AiShaderEvalParamVec(p_u));
      if (node_data->u_space == CS_Object)
      {
         AtVector uo = u;
         AiM4VectorByMatrixMult(&u, sg->M, &uo);
         u = AiV3Normalize(u);
      }
   }
   else if (!AiV3IsZero(sg->dPdu))
   {
      u = AiV3Normalize(sg->dPdu);
   }
   else
   {
      AiBuildLocalFramePolar(&u, &v, &N);
      computeV = false;
   }
   
   if (computeV)
   {
      v = AiV3Normalize(AiV3Cross(N, u));
      u = AiV3Cross(v, N);
   }
   
   brdf_data->evalSample = AiAshikhminShirleyMISSample;
   brdf_data->evalBrdf = AiAshikhminShirleyMISBRDF;
   brdf_data->evalPdf = AiAshikhminShirleyMISPDF;
   brdf_data->data = AiAshikhminShirleyMISCreateData(sg, &u, &v, gx, gy);
   
   sg->out.PTR = brdf_data;
}
