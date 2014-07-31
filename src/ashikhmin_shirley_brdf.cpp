#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(AshikhminShirleyBrdfMtd);

enum AshikhminShirleyBrdfParams
{
   p_glossiness_x = 0,
   p_glossiness_y,
   p_local_frame,
   p_custom_frame,
   p_frame_rotation,
   p_angle_units,
};

enum LocalFrame
{
   LF_Nf_dPdu = 0,
   LF_Nf_dPdv,
   LF_polar,
   LF_shirley,
   LF_custom
};

static const char* LocalFrameNames[] = { "Nf_dPdu", "Nf_dPdv", "polar", "shirley", "custom", NULL };

node_parameters
{
   AiParameterFlt("glossiness_x", 0.467f);
   AiParameterFlt("glossiness_y", 0.467f);
   AiParameterEnum("local_frame", LF_Nf_dPdu, LocalFrameNames);
   AiParameterMtx("custom_frame", AI_M4_IDENTITY);
   AiParameterFlt("frame_rotation", 0.0f);
   AiParameterEnum("angle_units", AU_Degrees, AngleUnitsNames);
   
   AiMetaDataSetBool(mds, "local_frame", "linkable", false);
   AiMetaDataSetBool(mds, "angle_units", "linkable", false);
}

struct AshikhminShirleyData
{
   bool glossiness_x_is_linked;
   float glossiness_x;
   bool glossiness_y_is_linked;
   float glossiness_y;
   LocalFrame local_frame;
   bool custom_frame_is_linked;
   AtMatrix custom_frame;
   bool frame_rotation_is_linked;
   float frame_rotation;
   float angle_scale;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(AshikhminShirleyData)));
}

node_update
{
   AshikhminShirleyData *data = (AshikhminShirleyData*) AiNodeGetLocalData(node);
   
   data->glossiness_x_is_linked = AiNodeIsLinked(node, "glossiness_x");
   data->glossiness_y_is_linked = AiNodeIsLinked(node, "glossiness_y");
   data->custom_frame_is_linked = AiNodeIsLinked(node, "custom_frame");
   data->frame_rotation_is_linked = AiNodeIsLinked(node, "frame_rotation");
   
   data->local_frame = (LocalFrame) AiNodeGetInt(node, "local_frame");
   data->angle_scale = (AiNodeGetInt(node, "angle_units") == AU_Degrees ? AI_DTOR : 1.0f);
   
   if (!data->glossiness_x_is_linked)
   {
      data->glossiness_x = AiNodeGetFlt(node, "glossiness_x");
   }
   
   if (!data->glossiness_y_is_linked)
   {
      data->glossiness_y = AiNodeGetFlt(node, "glossiness_y");
   }
   
   if (!data->custom_frame_is_linked)
   {
      AiNodeGetMatrix(node, "custom_frame", data->custom_frame);
   }
   
   if (!data->frame_rotation)
   {
      data->frame_rotation = AiNodeGetFlt(node, "frame_rotation");
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   AshikhminShirleyData *data = (AshikhminShirleyData*) AiNodeGetLocalData(node);
   
   float gx = (data->glossiness_x_is_linked ? AiShaderEvalParamFlt(p_glossiness_x) : data->glossiness_x);
   float gy = (data->glossiness_y_is_linked ? AiShaderEvalParamFlt(p_glossiness_y) : data->glossiness_y);
   
   AtVector U, V;
   
   if (data->local_frame == LF_Nf_dPdu)
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
   else if (data->local_frame == LF_Nf_dPdv)
   {
      if (!AiV3IsZero(sg->dPdv))
      {
         U = AiV3Normalize(sg->dPdv);
         V = AiV3Normalize(AiV3Cross(sg->Nf, U));
         U = AiV3Cross(V, sg->Nf);
      }
      else
      {
         AiBuildLocalFramePolar(&U, &V, &(sg->Nf));
      }
   }
   else if (data->local_frame == LF_polar)
   {
      AiBuildLocalFramePolar(&U, &V, &(sg->Nf));
   }
   else if (data->local_frame == LF_shirley)
   {
      AiBuildLocalFrameShirley(&U, &V, &(sg->Nf));
   }
   else
   {
      AtMatrix *frame = (data->custom_frame_is_linked ? AiShaderEvalParamMtx(p_custom_frame) : &(data->custom_frame));
      
      U.x = (*frame)[0][0];
      U.y = (*frame)[1][0];
      U.z = (*frame)[2][0];
      
      V.x = (*frame)[0][1];
      V.y = (*frame)[1][1];
      V.z = (*frame)[2][1];
   }
   
   float a = data->angle_scale * (data->frame_rotation_is_linked ? AiShaderEvalParamFlt(p_frame_rotation) : data->frame_rotation);
   if (fabs(a) > AI_EPSILON)
   {
      float cosa = cosf(a);
      float sina = sinf(a);
      AtVector u = U;
      AtVector v = V;
      U = cosa * u + sina * v;
      V = cosa * v - sina * u;
   }
   
   BRDFData *brdf_data = (BRDFData*) AiShaderGlobalsQuickAlloc(sg, sizeof(BRDFData));
   
   brdf_data->evalSample = AiAshikhminShirleyMISSample;
   brdf_data->evalBrdf = AiAshikhminShirleyMISBRDF;
   brdf_data->evalPdf = AiAshikhminShirleyMISPDF;
   brdf_data->data = AiAshikhminShirleyMISCreateData(sg, &U, &V, gx, gy);
   
   sg->out.PTR = brdf_data;
}
