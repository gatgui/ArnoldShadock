#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVTransformMtd);

enum UVTransformParams
{
   p_input = 0,
   p_scale,
   p_scale_pivot,
   p_rotation,
   p_rotation_pivot,
   p_translation,
   p_recompute_surface_uv_derivs
};

void ScaleUV(const AtPoint2 &pivot, const AtPoint2 &s, AtPoint2 &uv)
{
   uv -= pivot;
   uv.x *= s.x;
   uv.y *= s.y;
   uv += pivot;
}

void RotateUV(const AtPoint2 &pivot, float cosA, float sinA, AtPoint2 &uv)
{
   AtPoint2 tmp = uv - pivot;
   
   uv.x = pivot.x + cosA * tmp.x - sinA * tmp.y;
   uv.y = pivot.x + cosA * tmp.y + sinA * tmp.x;
}

void TranslateUV(const AtPoint2 &t, AtPoint2 &uv)
{
   uv += t;
}

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterPnt2("scale", 1.0f, 1.0f);
   AiParameterPnt2("scale_pivot", 0.0f, 0.0f);
   AiParameterFlt("rotation", 0.0f);
   AiParameterPnt2("rotation_pivot", 0.0f, 0.0f);
   AiParameterPnt2("translation", 0.0f, 0.0f);
   AiParameterBool("recompute_surface_uv_derivs", false);
   
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
   UVData uvs(sg);
   
   AtPoint2 S = AiShaderEvalParamPnt2(p_scale);
   AtPoint2 Sp = AiShaderEvalParamPnt2(p_scale_pivot);
   float R = AiShaderEvalParamFlt(p_rotation);
   AtPoint2 Rp = AiShaderEvalParamPnt2(p_rotation_pivot);
   AtPoint2 T = AiShaderEvalParamPnt2(p_translation);
   bool recompute_surface_uv_derivs = AiShaderEvalParamBool(p_recompute_surface_uv_derivs);
   
   AtPoint2 uv, dx, dy;
   AtVector dPdx, dPdy;
   float cosA, sinA;
   
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
   
   ScaleUV(Sp, S, uv);
   ScaleUV(Sp, S, dx);
   ScaleUV(Sp, S, dy);
   
   R *= AI_DTOR;
   cosA = cosf(R);
   sinA = sinf(R);
   RotateUV(Rp, cosA, sinA, uv);
   RotateUV(Rp, cosA, sinA, dx);
   RotateUV(Rp, cosA, sinA, dy);
   
   TranslateUV(T, uv);
   TranslateUV(T, dx);
   TranslateUV(T, dy);
   
   sg->u = uv.x;
   sg->v = uv.x;
   sg->dudx = dx.x - uv.x;
   sg->dvdx = dx.y - uv.y;
   sg->dudy = dy.x - uv.x;
   sg->dvdy = dy.y - uv.y;
   
   if (recompute_surface_uv_derivs)
   {
      // Re-compute dPdu and dPdv from dPdx, dPdy and new uv derivatives
      ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
   }
   
   sg->out.RGB = AiShaderEvalParamRGB(p_input);
   
   uvs.restore(sg);
}
