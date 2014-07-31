#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MakeFrameMtd);

enum MakeFrameParams
{
   p_method = 0,
   p_origin,
   p_custom_origin,
   p_z_axis,
   p_custom_axis
};

enum Origin
{
   O_P,
   O_custom
};

enum ZAxis
{
   ZA_N,
   ZA_Nf,
   ZA_custom
};

enum Method
{
   M_polar = 0,
   M_shirley,
   M_x_along_dPdu,
   M_x_along_dPdv
};

static const char* OriginNames[] = { "P", "custom", NULL };

static const char* ZAxisNames[] = { "N", "Nf", "custom", NULL };

static const char* MethodNames[] = { "polar", "shirley", "x_along_dPdu", "x_along_dPdv", NULL };

node_parameters
{
   AiParameterEnum("method", M_x_along_dPdu, MethodNames);
   AiParameterEnum("origin", O_P, OriginNames);
   AiParameterPnt("custom_origin", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("z_axis", ZA_Nf, ZAxisNames);
   AiParameterVec("custom_axis", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "method", "linkable", false);
   AiMetaDataSetBool(mds, "origin", "linkable", false);
   AiMetaDataSetBool(mds, "z_axis", "linkable", false);
}

struct MakeFrameData
{
   Method method;
   Origin origin;
   bool custom_origin_is_linked;
   AtVector custom_origin;
   ZAxis z_axis;
   bool custom_axis_is_linked;
   AtVector custom_axis;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(MakeFrameData)));
}

node_update
{
   MakeFrameData *data = (MakeFrameData*) AiNodeGetLocalData(node);
   
   data->method = (Method) AiNodeGetInt(node, "method");
   data->origin = (Origin) AiNodeGetInt(node, "origin");
   data->custom_origin_is_linked = AiNodeIsLinked(node, "custom_origin");
   data->z_axis = (ZAxis) AiNodeGetInt(node, "z_axis");
   data->custom_axis_is_linked = AiNodeIsLinked(node, "custom_axis");
   
   if (data->origin == O_custom && !data->custom_origin_is_linked)
   {
      data->custom_origin = AiNodeGetPnt(node, "custom_origin");
   }
   
   if (data->z_axis == ZA_custom && !data->custom_axis_is_linked)
   {
      data->custom_axis = AiV3Normalize(AiNodeGetVec(node, "custom_axis"));
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   MakeFrameData *data = (MakeFrameData*) AiNodeGetLocalData(node);
   
   AtVector O, X, Y, Z;
   
   // Read origin
   if (data->origin == O_P)
   {
      O = sg->P;
   }
   else if (!data->custom_origin_is_linked)
   {
      O = data->custom_origin;
   }
   else
   {
      O = AiShaderEvalParamPnt(p_custom_origin);
   }
   
   // Read Z axis
   if (data->z_axis == ZA_N)
   {
      Z = sg->N;
   }
   else if (data->z_axis == ZA_Nf)
   {
      Z = sg->Nf;
   }
   else if (!data->custom_axis_is_linked)
   {
      Z = data->custom_axis;
   }
   else
   {
      Z = AiV3Normalize(AiShaderEvalParamVec(p_custom_axis));
   }
   
   // Build X and Y axis
   switch (data->method)
   {
   case M_x_along_dPdu:
      if (!AiV3IsZero(sg->dPdu))
      {
         X = AiV3Normalize(sg->dPdu);
         Y = AiV3Normalize(AiV3Cross(Z, X));
         X = AiV3Cross(Y, Z);
      }
      else
      {
         AiBuildLocalFramePolar(&X, &Y, &Z);
      }
      break;
   case M_x_along_dPdv:
      if (!AiV3IsZero(sg->dPdv))
      {
         X = AiV3Normalize(sg->dPdv);
         Y = AiV3Normalize(AiV3Cross(Z, X));
         X = AiV3Cross(Y, Z);
      }
      else
      {
         AiBuildLocalFramePolar(&X, &Y, &Z);
      }
      break;
   case M_shirley:
      AiBuildLocalFrameShirley(&X, &Y, &Z);
      break;
   case M_polar:
   default:
      AiBuildLocalFramePolar(&X, &Y, &Z);
      break;
   }
   
   sg->out.pMTX = (AtMatrix*) AiShaderGlobalsQuickAlloc(sg, sizeof(AtMatrix));
   
   AiM4Frame(*(sg->out.pMTX), &O, &X, &Y, &Z);
}
