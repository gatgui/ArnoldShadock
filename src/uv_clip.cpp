#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVClipMtd);

enum UVClipParams
{
   p_input = 0,
   p_order,
   p_scale,
   p_scale_pivot,
   p_rotation,
   p_rotation_pivot,
   p_translation,
   p_transform_pivots,
   p_u_range,
   p_v_range,
   p_clipped_input
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterEnum(SSTR::order, TO_SRT, TransformOrderNames);
   AiParameterPnt2(SSTR::scale, 1.0f, 1.0f);
   AiParameterPnt2(SSTR::scale_pivot, 0.5f, 0.5f);
   AiParameterFlt(SSTR::rotation, 0.0f);
   AiParameterPnt2(SSTR::rotation_pivot, 0.5f, 0.5f);
   AiParameterPnt2(SSTR::translation, 0.0f, 0.0f);
   AiParameterBool(SSTR::transform_pivots, false);
   AiParameterPnt2("u_range", 0.0f, 1.0f);
   AiParameterPnt2("v_range", 0.0f, 1.0f);
   AiParameterRGBA("clipped_input", 0.0f, 0.0f, 0.0f, 1.0f);
}

struct UVClipData
{
   TransformOrder order;
   bool transformPivots;
};

node_initialize
{
   AiNodeSetLocalData(node, new UVClipData());
   AddMemUsage<UVClipData>();
}

node_update
{
   UVClipData *data = (UVClipData*) AiNodeGetLocalData(node);
   
   data->order = (TransformOrder) AiNodeGetInt(node, SSTR::order);
   data->transformPivots = AiNodeGetBool(node, SSTR::transform_pivots);
}

node_finish
{
   UVClipData *data = (UVClipData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<UVClipData>();
}

shader_evaluate
{
   UVClipData *data = (UVClipData*) AiNodeGetLocalData(node);

   AtPoint2 S = AiShaderEvalParamPnt2(p_scale);
   AtPoint2 Sp = AiShaderEvalParamPnt2(p_scale_pivot);
   float R = AiShaderEvalParamFlt(p_rotation);
   AtPoint2 Rp = AiShaderEvalParamPnt2(p_rotation_pivot);
   AtPoint2 T = AiShaderEvalParamPnt2(p_translation);
   AtPoint2 u_range = AiShaderEvalParamPnt2(p_u_range);
   AtPoint2 v_range = AiShaderEvalParamPnt2(p_v_range);
   
   AtPoint2 uv;
   float cosA, sinA;
   
   S.x = 1.0f / S.x;
   S.y = 1.0f / S.y;
   R = -R;
   T = -T;
   
   uv.x = sg->u;
   uv.y = sg->v;
   
   R *= AI_DTOR;
   cosA = cosf(R);
   sinA = sinf(R);
   
   switch (data->order)
   {
   case TO_SRT:
      ScaleUV(Sp, S, uv);
      if (data->transformPivots) ScaleUV(Sp, S, Rp);
      RotateUV(Rp, cosA, sinA, uv);
      TranslateUV(T, uv);
      break;
   case TO_STR:
      ScaleUV(Sp, S, uv);
      if (data->transformPivots) ScaleUV(Sp, S, Rp);
      TranslateUV(T, uv);
      if (data->transformPivots) TranslateUV(T, Rp);
      RotateUV(Rp, cosA, sinA, uv);
      break;
   case TO_RST:
      RotateUV(Rp, cosA, sinA, uv);
      if (data->transformPivots) RotateUV(Rp, cosA, sinA, Sp);
      ScaleUV(Sp, S, uv);
      TranslateUV(T, uv);
      break;
   case TO_RTS:
      RotateUV(Rp, cosA, sinA, uv);
      if (data->transformPivots) RotateUV(Rp, cosA, sinA, Sp);
      TranslateUV(T, uv);
      if (data->transformPivots) TranslateUV(T, Sp);
      ScaleUV(Sp, S, uv);
      break;
   case TO_TSR:
      TranslateUV(T, uv);
      if (data->transformPivots) { TranslateUV(T, Sp); TranslateUV(T, Rp); }
      ScaleUV(Sp, S, uv);
      if (data->transformPivots) ScaleUV(Sp, S, Rp);
      RotateUV(Rp, cosA, sinA, uv);
      break;
   case TO_TRS:
      TranslateUV(T, uv);
      if (data->transformPivots) { TranslateUV(T, Sp); TranslateUV(T, Rp); }
      RotateUV(Rp, cosA, sinA, uv);
      if (data->transformPivots) RotateUV(Rp, cosA, sinA, Sp);
      ScaleUV(Sp, S, uv);
      break;
   default:
      break;
   }
   
   if (uv.x < u_range.x || uv.x > u_range.y ||
       uv.y < v_range.x || uv.y > v_range.y)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_clipped_input);
   }
   else
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}
