#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(AshikhminShirleyBrdfMtd);

enum AshikhminShirleyBrdfParams
{
   p_glossiness_x = 0,
   p_glossiness_y,
   p_local_frame,
   p_custom_frame
};

enum LocalFrame
{
   LF_UV,
   LF_polar,
   LF_shirley,
   LF_custom
};

static const char* LocalFrameNames[] = { "UV", "polar", "shirley", "custom", NULL };

node_parameters
{
   AiParameterFlt("glossiness_x", 0.467f);
   AiParameterFlt("glossiness_y", 0.467f);
   AiParameterEnum("local_frame", LF_UV, LocalFrameNames);
   AiParameterMtx("custom_frame", AI_M4_IDENTITY);
   
   AiMetaDataSetBool(mds, "local_frame", "linkable", false);
}

node_initialize
{
}

node_update
{
   AiNodeSetLocalData(node, (void*) AiNodeGetInt(node, "local_frame"));
}

node_finish
{
}

shader_evaluate
{
   void *local_frame = AiNodeGetLocalData(node);
   
   float gx = AiShaderEvalParamFlt(p_glossiness_x);
   float gy = AiShaderEvalParamFlt(p_glossiness_y);
   
   AtVector U, V;
   
   if (local_frame == (void*)LF_UV)
   {
      if (!AiV3IsZero(sg->dPdu))
      {
         U = AiV3Normalize(sg->dPdu);
         V = AiV3Normalize(AiV3Cross(sg->Nf, U));
         U = AiV3Cross(V, sg->Nf);
      }
      else
      {
         AiBuildLocalFramePolar(&U, &V, &(sg->Nf));
      }
   }
   else if (local_frame == (void*)LF_polar)
   {
      AiBuildLocalFramePolar(&U, &V, &(sg->Nf));
   }
   else if (local_frame == (void*)LF_shirley)
   {
      AiBuildLocalFrameShirley(&U, &V, &(sg->Nf));
   }
   else
   {
      AtMatrix *frame = AiShaderEvalParamMtx(p_custom_frame);
      
      U.x = (*frame)[0][0];
      U.y = (*frame)[0][1];
      U.z = (*frame)[0][2];
      
      V.x = (*frame)[1][0];
      V.y = (*frame)[1][1];
      V.z = (*frame)[1][2];
   }
   
   BRDFData *brdf_data = (BRDFData*) AiShaderGlobalsQuickAlloc(sg, sizeof(BRDFData));
   
   brdf_data->evalSample = AiAshikhminShirleyMISSample;
   brdf_data->evalBrdf = AiAshikhminShirleyMISBRDF;
   brdf_data->evalPdf = AiAshikhminShirleyMISPDF;
   brdf_data->data = AiAshikhminShirleyMISCreateData(sg, &U, &V, gx, gy);
   
   sg->out.PTR = brdf_data;
}
