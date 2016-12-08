#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BrdfAshikhminShirleyMtd);

enum BrdfAshikhminShirleyParams
{
   p_glossiness_x = 0,
   p_glossiness_y,
   p_local_frame,
   p_custom_frame,
   p_frame_rotation,
   p_angle_units,
};

node_parameters
{
   AiParameterFlt(SSTR::glossiness_x, 0.467f);
   AiParameterFlt(SSTR::glossiness_y, 0.467f);
   AiParameterEnum(SSTR::local_frame, LF_Nf_dPdu, LocalFrameNames);
   AiParameterMtx(SSTR::custom_frame, AI_M4_IDENTITY);
   AiParameterFlt(SSTR::frame_rotation, 0.0f);
   AiParameterEnum(SSTR::angle_units, AU_Degrees, AngleUnitsNames);
}

struct BrdfAshikhminShirleyData
{
   bool evalGlossinessX;
   float glossinessX;
   bool evalGLossinessY;
   float glossinessY;
   LocalFrame localFrame;
   bool evalCustomFrame;
   AtMatrix customFrame;
   bool evalFrameRotation;
   float frameRotation;
   float angleScale;
};

node_initialize
{
   AiNodeSetLocalData(node, new BrdfAshikhminShirleyData());
   AddMemUsage<BrdfAshikhminShirleyData>();
}

node_update
{
   BrdfAshikhminShirleyData *data = (BrdfAshikhminShirleyData*) AiNodeGetLocalData(node);
   
   data->evalGlossinessX = AiNodeIsLinked(node, SSTR::glossiness_x);
   data->evalGLossinessY = AiNodeIsLinked(node, SSTR::glossiness_y);
   data->evalCustomFrame = AiNodeIsLinked(node, SSTR::custom_frame);
   data->evalFrameRotation = AiNodeIsLinked(node, SSTR::frame_rotation);
   
   data->localFrame = (LocalFrame) AiNodeGetInt(node, SSTR::local_frame);
   data->angleScale = (AiNodeGetInt(node, SSTR::angle_units) == AU_Degrees ? AI_DTOR : 1.0f);
   
   if (!data->evalGlossinessX)
   {
      data->glossinessX = AiNodeGetFlt(node, SSTR::glossiness_x);
   }
   
   if (!data->evalGLossinessY)
   {
      data->glossinessY = AiNodeGetFlt(node, SSTR::glossiness_y);
   }
   
   if (!data->evalCustomFrame)
   {
      AiNodeGetMatrix(node, SSTR::custom_frame, data->customFrame);
   }
   
   if (!data->evalFrameRotation)
   {
      data->frameRotation = AiNodeGetFlt(node, SSTR::frame_rotation);
   }
}

node_finish
{
   BrdfAshikhminShirleyData *data = (BrdfAshikhminShirleyData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<BrdfAshikhminShirleyData>();
}

shader_evaluate
{
   BrdfAshikhminShirleyData *data = (BrdfAshikhminShirleyData*) AiNodeGetLocalData(node);
   
   float gx = (data->evalGlossinessX ? AiShaderEvalParamFlt(p_glossiness_x) : data->glossinessX);
   float gy = (data->evalGLossinessY ? AiShaderEvalParamFlt(p_glossiness_y) : data->glossinessY);
   
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
   
   BRDFData *brdfData = (BRDFData*) AiShaderGlobalsQuickAlloc(sg, sizeof(BRDFData));
   
   brdfData->evalSample = AiAshikhminShirleyMISSample;
   brdfData->evalBrdf = AiAshikhminShirleyMISBRDF;
   brdfData->evalPdf = AiAshikhminShirleyMISPDF;
   brdfData->data = AiAshikhminShirleyMISCreateData(sg, &U, &V, gx, gy);
   
   AiStateSetMsgPtr(SSTR::agsb_brdf, brdfData);
   
   sg->out.RGB = AI_RGB_BLACK;
}
