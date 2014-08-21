#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BtdfMicrofacetIntegrateMtd);

enum BtdfMicrofacetIntegrateParams
{
   p_roughness_x = 0,
   p_roughness_y,
   p_local_frame,
   p_custom_frame,
   p_frame_rotation,
   p_angle_units,
   p_eta_i,
   p_eta_o,
   p_transmittance
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
   AiParameterFlt("roughness_x", 0.467f);
   AiParameterFlt("roughness_y", 0.467f);
   AiParameterEnum("local_frame", LF_Nf_dPdu, LocalFrameNames);
   AiParameterMtx("custom_frame", AI_M4_IDENTITY);
   AiParameterFlt("frame_rotation", 0.0f);
   AiParameterEnum("angle_units", AU_Degrees, AngleUnitsNames);
   AiParameterFlt("eta_i", 1.0);
   AiParameterFlt("eta_o", 1.0);
   AiParameterRGB("transmittance", 1.0f, 1.0f, 1.0f);
   
   AiMetaDataSetBool(mds, "local_frame", "linkable", false);
   AiMetaDataSetBool(mds, "angle_units", "linkable", false);
   //AiMetaDataSetFlt(mds, "eta_i", "min", 1.0f);
   //AiMetaDataSetFlt(mds, "eta_o", "min", 1.0f);
}

struct CookTorranceData
{
   bool roughness_x_is_linked;
   float roughness_x;
   bool roughness_y_is_linked;
   float roughness_y;
   LocalFrame local_frame;
   bool custom_frame_is_linked;
   AtMatrix custom_frame;
   bool frame_rotation_is_linked;
   float frame_rotation;
   float angle_scale;
   bool eta_i_is_linked;
   float eta_i;
   bool eta_o_is_linked;
   float eta_o;
   bool transmittance_is_linked;
   AtColor transmittance;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(CookTorranceData)));
}

node_update
{
   CookTorranceData *data = (CookTorranceData*) AiNodeGetLocalData(node);
   
   data->roughness_x_is_linked = AiNodeIsLinked(node, "roughness_x");
   data->roughness_y_is_linked = AiNodeIsLinked(node, "roughness_y");
   data->custom_frame_is_linked = AiNodeIsLinked(node, "custom_frame");
   data->frame_rotation_is_linked = AiNodeIsLinked(node, "frame_rotation");
   data->eta_i_is_linked = AiNodeIsLinked(node, "eta_i");
   data->eta_o_is_linked = AiNodeIsLinked(node, "eta_o");
   data->transmittance_is_linked = AiNodeIsLinked(node, "transmittance");
   
   data->local_frame = (LocalFrame) AiNodeGetInt(node, "local_frame");
   data->angle_scale = (AiNodeGetInt(node, "angle_units") == AU_Degrees ? AI_DTOR : 1.0f);
   
   if (!data->roughness_x_is_linked)
   {
      data->roughness_x = AiNodeGetFlt(node, "roughness_x");
   }
   
   if (!data->roughness_y_is_linked)
   {
      data->roughness_y = AiNodeGetFlt(node, "roughness_y");
   }
   
   if (!data->custom_frame_is_linked)
   {
      AiNodeGetMatrix(node, "custom_frame", data->custom_frame);
   }
   
   if (!data->frame_rotation_is_linked)
   {
      data->frame_rotation = AiNodeGetFlt(node, "frame_rotation");
   }
   
   if (!data->eta_i_is_linked)
   {
      data->eta_i = AiNodeGetFlt(node, "eta_i");
   }
   
   if (!data->eta_o_is_linked)
   {
      data->eta_o = AiNodeGetFlt(node, "eta_o");
   }
   
   if (!data->transmittance_is_linked)
   {
      data->transmittance = AiNodeGetRGB(node, "transmittance");
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   CookTorranceData *data = (CookTorranceData*) AiNodeGetLocalData(node);
   
   float rx = (data->roughness_x_is_linked ? AiShaderEvalParamFlt(p_roughness_x) : data->roughness_x);
   float ry = (data->roughness_y_is_linked ? AiShaderEvalParamFlt(p_roughness_y) : data->roughness_y);
   
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
      U.y = (*frame)[0][1];
      U.z = (*frame)[0][2];
      
      V.x = (*frame)[1][0];
      V.y = (*frame)[1][1];
      V.z = (*frame)[1][2];
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
   
   float eta_i = (data->eta_i_is_linked ? AiShaderEvalParamFlt(p_eta_i) : data->eta_i);
   
   float eta_o = (data->eta_o_is_linked ? AiShaderEvalParamFlt(p_eta_o) : data->eta_o);
   
   AtColor transmittance = (data->transmittance_is_linked ? AiShaderEvalParamRGB(p_transmittance) : data->transmittance);
   
   sg->out.RGB = AiMicrofacetBTDFIntegrate(&(sg->N), sg, &U, &V, rx, ry, eta_i, eta_o, transmittance);
}
