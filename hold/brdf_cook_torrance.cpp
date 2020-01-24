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

AI_SHADER_NODE_EXPORT_METHODS(BrdfCookTorranceMtd);

enum BrdfCookTorranceParams
{
   p_roughness_x = 0,
   p_roughness_y,
   p_local_frame,
   p_custom_frame,
   p_frame_rotation,
   p_angle_units
};

node_parameters
{
   AiParameterFlt(SSTR::roughness_x, 0.467f);
   AiParameterFlt(SSTR::roughness_y, 0.467f);
   AiParameterEnum(SSTR::local_frame, LF_Nf_dPdu, LocalFrameNames);
   AiParameterMtx(SSTR::custom_frame, AI_M4_IDENTITY);
   AiParameterFlt(SSTR::frame_rotation, 0.0f);
   AiParameterEnum(SSTR::angle_units, AU_Degrees, AngleUnitsNames);
}

struct BrdfCookTorranceData
{
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
};

node_initialize
{
   AiMsgWarning("[shading_block] 'brdf_cook_torrance' is deprecated, use 'brdf_microfacet' instead with distribution set to 'Beckmann'.");
   AiNodeSetLocalData(node, new BrdfCookTorranceData());
   AddMemUsage<BrdfCookTorranceData>();
}

node_update
{
   BrdfCookTorranceData *data = (BrdfCookTorranceData*) AiNodeGetLocalData(node);
   
   data->evalRoughnessX = AiNodeIsLinked(node, SSTR::roughness_x);
   data->evalRoughnessY = AiNodeIsLinked(node, SSTR::roughness_y);
   data->evalCustomFrame = AiNodeIsLinked(node, SSTR::custom_frame);
   data->evalFrameRotation = AiNodeIsLinked(node, SSTR::frame_rotation);
   
   data->localFrame = (LocalFrame) AiNodeGetInt(node, SSTR::local_frame);
   data->angleScale = (AiNodeGetInt(node, SSTR::angle_units) == AU_Degrees ? AI_DTOR : 1.0f);
   
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
}

node_finish
{
   BrdfCookTorranceData *data = (BrdfCookTorranceData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<BrdfCookTorranceData>();
}

shader_evaluate
{
   BrdfCookTorranceData *data = (BrdfCookTorranceData*) AiNodeGetLocalData(node);
   
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
   
   BRDFData *brdfData = (BRDFData*) AiShaderGlobalsQuickAlloc(sg, sizeof(BRDFData));
   
   brdfData->evalSample = AiCookTorranceMISSample;
   brdfData->evalBrdf = AiCookTorranceMISBRDF;
   brdfData->evalPdf = AiCookTorranceMISPDF;
   brdfData->data = AiCookTorranceMISCreateData(sg, &U, &V, rx, ry);
   
   AiStateSetMsgPtr(SSTR::agsb_brdf, brdfData);
   
   sg->out.RGB = AI_RGB_BLACK;
}
