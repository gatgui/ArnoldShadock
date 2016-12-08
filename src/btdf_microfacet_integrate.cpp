#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BtdfMicrofacetIntegrateMtd);

enum BtdfMicrofacetIntegrateParams
{
   p_weight = 0,
   p_distribution,
   p_roughness_x,
   p_roughness_y,
   p_local_frame,
   p_custom_frame,
   p_frame_rotation,
   p_angle_units,
   p_ior,
   p_dispersion,
   p_fresnel,
   p_transmittance
};

node_parameters
{
   AiParameterRGB(SSTR::weight, 1.0f, 1.0f, 1.0f);
   AiParameterEnum(SSTR::distribution, MD_Beckmann, MicrofacetDistributionNames);
   AiParameterFlt(SSTR::roughness_x, 0.467f);
   AiParameterFlt(SSTR::roughness_y, 0.467f);
   AiParameterEnum(SSTR::local_frame, LF_Nf_dPdu, LocalFrameNames);
   AiParameterMtx(SSTR::custom_frame, AI_M4_IDENTITY);
   AiParameterFlt(SSTR::frame_rotation, 0.0f);
   AiParameterEnum(SSTR::angle_units, AU_Degrees, AngleUnitsNames);
   AiParameterFlt(SSTR::ior, 1.0f);
   AiParameterFlt(SSTR::dispersion, 0.0f);
   AiParameterBool(SSTR::fresnel, false);
   AiParameterRGB(SSTR::transmittance, 1.0f, 1.0f, 1.0f);
}

struct BtdfMicrofacetIntegrateData
{
   int distribution;
   bool evalRoughnessX;
   float roughnessX;
   bool evalRoughnessY;
   float roughnessY;
   LocalFrame localFrame;
   bool evalCustomFrame;
   AtMatrix customFrame;
   bool evalFrameRotation;
   float frameRotation;
   float angleScale;
   bool evalIor;
   float ior;
   bool evalDispersion;
   float dispersion;
   bool fresnel;
   bool evalTransmittance;
   AtColor transmittance;
};

node_initialize
{
   AiNodeSetLocalData(node, new BtdfMicrofacetIntegrateData());
   AddMemUsage<BtdfMicrofacetIntegrateData>();
}

node_update
{
   BtdfMicrofacetIntegrateData *data = (BtdfMicrofacetIntegrateData*) AiNodeGetLocalData(node);
   
   data->evalRoughnessX = AiNodeIsLinked(node, SSTR::roughness_x);
   data->evalRoughnessY = AiNodeIsLinked(node, SSTR::roughness_y);
   data->evalCustomFrame = AiNodeIsLinked(node, SSTR::custom_frame);
   data->evalFrameRotation = AiNodeIsLinked(node, SSTR::frame_rotation);
   data->evalIor = AiNodeIsLinked(node, SSTR::ior);
   data->evalDispersion = AiNodeIsLinked(node, SSTR::dispersion);
   data->evalTransmittance = AiNodeIsLinked(node, SSTR::transmittance);
   
   data->distribution = AiNodeGetInt(node, SSTR::distribution);
   data->localFrame = (LocalFrame) AiNodeGetInt(node, SSTR::local_frame);
   data->angleScale = (AiNodeGetInt(node, SSTR::angle_units) == AU_Degrees ? AI_DTOR : 1.0f);
   data->fresnel = AiNodeGetBool(node, SSTR::fresnel);
   
   if (!data->evalRoughnessX)
   {
      data->roughnessX = AiNodeGetFlt(node, SSTR::roughness_x);
   }
   
   if (!data->evalRoughnessY)
   {
      data->roughnessY = AiNodeGetFlt(node, SSTR::roughness_y);
   }
   
   if (!data->evalCustomFrame)
   {
      AiNodeGetMatrix(node, SSTR::custom_frame, data->customFrame);
   }
   
   if (!data->evalFrameRotation)
   {
      data->frameRotation = AiNodeGetFlt(node, SSTR::frame_rotation);
   }
   
   if (!data->evalIor)
   {
      data->ior = AiNodeGetFlt(node, SSTR::ior);
   }
   
   if (!data->evalDispersion)
   {
      data->dispersion = AiNodeGetFlt(node, SSTR::dispersion);
   }
   
   if (!data->evalTransmittance)
   {
      data->transmittance = AiNodeGetRGB(node, SSTR::transmittance);
   }
}

node_finish
{
   BtdfMicrofacetIntegrateData *data = (BtdfMicrofacetIntegrateData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<BtdfMicrofacetIntegrateData>();
}

shader_evaluate
{
   BtdfMicrofacetIntegrateData *data = (BtdfMicrofacetIntegrateData*) AiNodeGetLocalData(node);
   
   AtColor weight = AiShaderEvalParamRGB(p_weight);
   
   if (AiColorIsZero(weight))
   {
      sg->out.RGB = AI_RGB_BLACK;
      return;
   }
   
   float rx = (data->evalRoughnessX ? AiShaderEvalParamFlt(p_roughness_x) : data->roughnessX);
   float ry = (data->evalRoughnessY ? AiShaderEvalParamFlt(p_roughness_y) : data->roughnessY);
   
   AtVector U, V;
   
   if (data->localFrame == LF_Nf_dPdu)
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
   else if (data->localFrame == LF_Nf_dPdv)
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
   else if (data->localFrame == LF_polar)
   {
      AiBuildLocalFramePolar(&U, &V, &(sg->Nf));
   }
   else if (data->localFrame == LF_shirley)
   {
      AiBuildLocalFrameShirley(&U, &V, &(sg->Nf));
   }
   else
   {
      AtMatrix *frame = (data->evalCustomFrame ? AiShaderEvalParamMtx(p_custom_frame) : &(data->customFrame));
      
      U.x = (*frame)[0][0];
      U.y = (*frame)[0][1];
      U.z = (*frame)[0][2];
      
      V.x = (*frame)[1][0];
      V.y = (*frame)[1][1];
      V.z = (*frame)[1][2];
   }
   
   float a = data->angleScale * (data->evalFrameRotation ? AiShaderEvalParamFlt(p_frame_rotation) : data->frameRotation);
   if (fabs(a) > AI_EPSILON)
   {
      float cosa = cosf(a);
      float sina = sinf(a);
      AtVector u = U;
      AtVector v = V;
      U = cosa * u + sina * v;
      V = cosa * v - sina * u;
   }
   
   float ior = (data->evalIor ? AiShaderEvalParamFlt(p_ior) : data->ior);
   
   AtColor transmittance = (data->evalTransmittance ? AiShaderEvalParamRGB(p_transmittance) : data->transmittance);
   
   float dispersion = (data->evalDispersion ? AiShaderEvalParamFlt(p_dispersion) : data->dispersion);
   
   bool entering = (AiV3Dot(sg->N, sg->Rd) < 0.0f);
   
   sg->out.RGB = AiMicrofacetBTDFIntegrate(&(sg->N), sg, data->distribution, &U, rx, ry, ior, entering, dispersion, data->fresnel, transmittance, weight);
}
