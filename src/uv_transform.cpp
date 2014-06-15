#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVTransformMtd);

enum UVTransformParams
{
   p_input = 0,
   p_order,
   p_scale,
   p_scale_pivot,
   p_rotation,
   p_rotation_pivot,
   p_translation,
   p_transform_pivots,
   p_recompute_surface_uv_derivs
};

enum UVTransformOrder
{
   Order_SRT = 0,
   Order_STR,
   Order_RST,
   Order_RTS,
   Order_TSR,
   Order_TRS
};

static const char* UVTransformOrderNames[] =
{
   "SRT",
   "STR",
   "RST",
   "RTS",
   "TSR",
   "TRS",
   NULL
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("order", Order_SRT, UVTransformOrderNames);
   AiParameterPnt2("scale", 1.0f, 1.0f);
   AiParameterPnt2("scale_pivot", 0.5f, 0.5f);
   AiParameterFlt("rotation", 0.0f);
   AiParameterPnt2("rotation_pivot", 0.5f, 0.5f);
   AiParameterPnt2("translation", 0.0f, 0.0f);
   AiParameterBool("transform_pivots", false);
   AiParameterBool("recompute_surface_uv_derivs", false);
   
   AiMetaDataSetBool(mds, "order", "linkable", false);
   AiMetaDataSetBool(mds, "transform_pivots", "linkable", false);
   AiMetaDataSetBool(mds, "recompute_surface_uv_derivs", "linkable", false);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   AtPoint2 uv, dx, dy;
   AtVector dPdx, dPdy;
   float cosA, sinA;
   
   UVData uvs(sg);
   
   UVTransformOrder order = (UVTransformOrder) AiShaderEvalParamInt(p_order);
   AtPoint2 S = AiShaderEvalParamPnt2(p_scale);
   AtPoint2 Sp = AiShaderEvalParamPnt2(p_scale_pivot);
   float R = AiShaderEvalParamFlt(p_rotation);
   AtPoint2 Rp = AiShaderEvalParamPnt2(p_rotation_pivot);
   AtPoint2 T = AiShaderEvalParamPnt2(p_translation);
   bool transform_pivots = AiShaderEvalParamBool(p_transform_pivots);
   bool recompute_surface_uv_derivs = AiShaderEvalParamBool(p_recompute_surface_uv_derivs);
   
   // start by inverting values... we're modifying the uvs, be we visualize the resulting mapped texture
   // feels more natural to manipulate the result
   S.x = 1.0f / S.x;
   S.y = 1.0f / S.y;
   R = -R;
   T = -T;
   
   if (recompute_surface_uv_derivs)
   {
      ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   }
   
   uv.x = sg->u;
   uv.y = sg->v;
   dx.x = sg->u + sg->dudx;
   dx.y = sg->v + sg->dvdx;
   dy.x = sg->u + sg->dudy;
   dy.y = sg->v + sg->dvdy;
   
   R *= AI_DTOR;
   cosA = cosf(R);
   sinA = sinf(R);
   
   switch (order)
   {
   case Order_SRT:
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      if (transform_pivots) ScaleUV(Sp, S, Rp);
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      break;
   case Order_STR:
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      if (transform_pivots) ScaleUV(Sp, S, Rp);
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      if (transform_pivots) TranslateUV(T, Rp);
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      break;
   case Order_RST:
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      if (transform_pivots) RotateUV(Rp, cosA, sinA, Sp);
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      break;
   case Order_RTS:
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      if (transform_pivots) RotateUV(Rp, cosA, sinA, Sp);
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      if (transform_pivots) TranslateUV(T, Sp);
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      break;
   case Order_TSR:
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      if (transform_pivots) { TranslateUV(T, Sp); TranslateUV(T, Rp); }
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      if (transform_pivots) ScaleUV(Sp, S, Rp);
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      break;
   case Order_TRS:
      TranslateUV(T, uv);
      TranslateUV(T, dx);
      TranslateUV(T, dy);
      if (transform_pivots) { TranslateUV(T, Sp); TranslateUV(T, Rp); }
      RotateUV(Rp, cosA, sinA, uv);
      RotateUV(Rp, cosA, sinA, dx);
      RotateUV(Rp, cosA, sinA, dy);
      if (transform_pivots) RotateUV(Rp, cosA, sinA, Sp);
      ScaleUV(Sp, S, uv);
      ScaleUV(Sp, S, dx);
      ScaleUV(Sp, S, dy);
      break;
   default:
      break;
   }
   
   sg->u = uv.x;
   sg->v = uv.y;
   sg->dudx = dx.x - uv.x;
   sg->dvdx = dx.y - uv.y;
   sg->dvdy = dy.x - uv.x;
   sg->dvdy = dy.y - uv.y;
   
   if (recompute_surface_uv_derivs)
   {
      // Re-compute dPdu and dPdv from dPdx, dPdy and new uv derivatives
      ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
   }
   
   sg->out.RGB = AiShaderEvalParamRGB(p_input);
   
   uvs.restore(sg);
}
