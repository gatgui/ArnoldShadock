#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(LocalFrameMtd);

enum LocalFrameParams
{
   p_method = 0,
   p_N,
   p_output_axis
};

enum Method
{
   M_polar = 0,
   M_shirley,
   M_x_along_dPdu,
   M_x_along_dPdv
};

enum OutputAxis
{
   OA_x = 0,
   OA_y,
   OA_z
};

const char* MethodNames[] = { "polar", "shirley", "x_along_dPdu", "x_along_dPdv", NULL };

const char* OutputAxisNames[] = { "x", "y", "z", NULL };

node_parameters
{
   AiParameterEnum("method", M_polar, MethodNames);
   AiParameterVec("N", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("output_axis", OA_x, OutputAxisNames);
   
   AiMetaDataSetBool(mds, "method", "linkable", false);
   AiMetaDataSetBool(mds, "output_axis", "linkable", false);
}

struct LocalFrameData
{
   Method method;
   bool N_is_linked;
   OutputAxis output_axis;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(LocalFrameData)));
}

node_update
{
   LocalFrameData *data = (LocalFrameData*) AiNodeGetLocalData(node);
   
   data->method = (Method) AiNodeGetInt(node, "method");
   data->N_is_linked = AiNodeIsLinked(node, "N");
   data->output_axis = (OutputAxis) AiNodeGetInt(node, "output_vector");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   LocalFrameData *data = (LocalFrameData*) AiNodeGetLocalData(node);
   
   AtVector X, Y, Z = (data->N_is_linked ? AiV3Normalize(AiShaderEvalParamVec(p_N)) : sg->N);
   
   if (data->output_axis == OA_z)
   {
      sg->out.VEC = Z;
   }
   else
   {
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
      
      sg->out.VEC = (data->output_axis == OA_x ? X : Y);
   }
}
