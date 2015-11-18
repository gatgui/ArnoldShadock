#include <ai.h>
#include <cstring>
#include <cstdlib>
#include "common.h"

// ---

#ifndef SHADERS_PREFIX
#  define SHADERS_PREFIX ""
#endif

static int gsShaderCount = 0;

#define DECLARE_SHADER(name)\
   extern AtNodeMethods* name##Mtd;\
   static int name = gsShaderCount++

#define DECLARE_FLOAT_SHADER(name) DECLARE_SHADER(name##F)

#define DECLARE_COLOR_SHADER(name) DECLARE_SHADER(name##C3)

#define DECLARE_VECTOR_SHADER(name) DECLARE_SHADER(name##V)

#define DECLARE_FCV_SHADER(name)\
   DECLARE_FLOAT_SHADER(name);\
   DECLARE_COLOR_SHADER(name);\
   DECLARE_VECTOR_SHADER(name)

#define REGISTER_SHADER(s_name, a_name, a_type)\
   if (i == s_name)\
   {\
      node->name = SHADERS_PREFIX #a_name;\
      node->node_type = AI_NODE_SHADER;\
      node->output_type = a_type;\
      node->methods = s_name##Mtd;\
      strcpy(node->version, AI_VERSION);\
      return true;\
   }

#define REGISTER_FLOAT_SHADER(s_name, a_name) REGISTER_SHADER(s_name##F, a_name##_f, AI_TYPE_FLOAT)

#define REGISTER_COLOR_SHADER(s_name, a_name) REGISTER_SHADER(s_name##C3, a_name##_c3, AI_TYPE_RGB)

#define REGISTER_VECTOR_SHADER(s_name, a_name) REGISTER_SHADER(s_name##V, a_name##_v, AI_TYPE_VECTOR)

#define REGISTER_FCV_SHADER(s_name, a_name)\
   REGISTER_FLOAT_SHADER(s_name, a_name)\
   REGISTER_COLOR_SHADER(s_name, a_name)\
   REGISTER_VECTOR_SHADER(s_name, a_name)

// ---

DECLARE_FCV_SHADER(Abs);
DECLARE_FCV_SHADER(Add);
DECLARE_FCV_SHADER(Bias);
DECLARE_FCV_SHADER(BiasGain);
DECLARE_FCV_SHADER(Ceil);
DECLARE_FCV_SHADER(Clamp);
DECLARE_SHADER(CompareF);
DECLARE_SHADER(CompareC3);
DECLARE_SHADER(CompareV);
DECLARE_SHADER(FToB);
DECLARE_SHADER(FToI);
DECLARE_SHADER(VToC3);
DECLARE_SHADER(C3ToV);
DECLARE_SHADER(C3ToF);
DECLARE_SHADER(FToV);
DECLARE_SHADER(FToC3);
DECLARE_SHADER(FToC4);
DECLARE_SHADER(BToI);
DECLARE_SHADER(BToF);
DECLARE_SHADER(Cross);
DECLARE_FCV_SHADER(Gain);
DECLARE_FCV_SHADER(Div);
DECLARE_SHADER(Dot);
DECLARE_FCV_SHADER(Exp);
DECLARE_FCV_SHADER(Floor);
DECLARE_FCV_SHADER(Inv);
DECLARE_FCV_SHADER(Log);
DECLARE_FCV_SHADER(Max);
DECLARE_FCV_SHADER(Min);
DECLARE_FCV_SHADER(Mix);
DECLARE_FCV_SHADER(Mod);
DECLARE_FCV_SHADER(MultiSwitch);
DECLARE_FCV_SHADER(Mult);
DECLARE_FCV_SHADER(Neg);
DECLARE_SHADER(Normalize);
DECLARE_FCV_SHADER(Pow);
DECLARE_FCV_SHADER(Remap);
DECLARE_FCV_SHADER(Round);
DECLARE_FCV_SHADER(Switch);
DECLARE_FCV_SHADER(Sign);
DECLARE_FCV_SHADER(SmoothStep);
DECLARE_FCV_SHADER(Sqrt);
DECLARE_FCV_SHADER(Sub);
DECLARE_FLOAT_SHADER(Trigo);
DECLARE_VECTOR_SHADER(Trigo);
DECLARE_SHADER(Length);
DECLARE_SHADER(ShapeAttrB);
DECLARE_SHADER(ShapeAttrI);
DECLARE_SHADER(ShapeAttrF);
DECLARE_SHADER(ShapeAttrP2);
DECLARE_SHADER(ShapeAttrV);
DECLARE_SHADER(ShapeAttrC3);
DECLARE_SHADER(ShapeAttrC4);
DECLARE_SHADER(ShapeAttrS);
#ifdef USE_AGSTATE
DECLARE_SHADER(agIntState);
DECLARE_SHADER(agFloatState);
DECLARE_SHADER(agColorState);
DECLARE_SHADER(agVectorState);
DECLARE_SHADER(agMatrixState);
DECLARE_SHADER(agNodeState);
#endif
DECLARE_SHADER(Transform);
DECLARE_SHADER(MakeM);
DECLARE_SHADER(MultM);
DECLARE_SHADER(SetTraceSet);
DECLARE_SHADER(Shade);
DECLARE_SHADER(Holdout);
DECLARE_SHADER(ShadingPoint);
DECLARE_SHADER(ShadingNormal);
DECLARE_SHADER(WriteAOVs);
DECLARE_SHADER(ReadAOVB);
DECLARE_SHADER(ReadAOVI);
DECLARE_SHADER(ReadAOVF);
DECLARE_SHADER(ReadAOVP2);
DECLARE_SHADER(ReadAOVP3);
DECLARE_SHADER(ReadAOVV);
DECLARE_SHADER(ReadAOVC3);
DECLARE_SHADER(ReadAOVC4);
DECLARE_SHADER(InvM);
DECLARE_SHADER(Transpose);
DECLARE_SHADER(LerpM);
DECLARE_FCV_SHADER(Ramp);
DECLARE_FCV_SHADER(SmoothSwitch);
DECLARE_FCV_SHADER(RangeSwitch);
DECLARE_FCV_SHADER(SmoothRangeSwitch);
DECLARE_SHADER(MakeP2);
DECLARE_SHADER(MakeV);
DECLARE_SHADER(MakeC3);
DECLARE_SHADER(MakeC4);
DECLARE_FCV_SHADER(Gaussian);
#ifdef USE_AGNOISES
DECLARE_SHADER(Fractal);
DECLARE_SHADER(Voronoi);
DECLARE_SHADER(DistortPoint);
#endif
#ifdef USE_AGSEEXPR
DECLARE_SHADER(agSeExpr);
#endif
#ifdef USE_AGANIMCURVE
DECLARE_SHADER(agAnimCurve);
#endif
#ifdef USE_AGUSERDATARAMP
DECLARE_SHADER(agUserDataFloatRamp);
DECLARE_SHADER(agUserDataColorRamp);
DECLARE_SHADER(agUserDataVectorRamp);
#endif
DECLARE_SHADER(UVSwitch);
DECLARE_SHADER(UVProject);
DECLARE_SHADER(UVTransform);
DECLARE_SHADER(UVClip);
DECLARE_SHADER(MakeTx);
DECLARE_SHADER(CameraTransform);
DECLARE_SHADER(Resolution);
DECLARE_SHADER(TokenPath);
DECLARE_SHADER(Texture);
DECLARE_SHADER(WriteMsgs);
DECLARE_SHADER(ReadMsgB);
DECLARE_SHADER(ReadMsgI);
DECLARE_SHADER(ReadMsgF);
DECLARE_SHADER(ReadMsgP2);
DECLARE_SHADER(ReadMsgP3);
DECLARE_SHADER(ReadMsgV);
DECLARE_SHADER(ReadMsgC3);
DECLARE_SHADER(ReadMsgC4);
DECLARE_SHADER(IlluminanceLoop);
DECLARE_SHADER(LightWeight);
DECLARE_SHADER(ReflectV);
DECLARE_SHADER(RefractV);
DECLARE_FLOAT_SHADER(Fresnel);
DECLARE_COLOR_SHADER(Fresnel);
DECLARE_SHADER(BrdfOrenNayar);
DECLARE_SHADER(BrdfCookTorrance);
DECLARE_SHADER(BrdfWardDuer);
DECLARE_SHADER(BrdfAshikhminShirley);
DECLARE_SHADER(EvalLightSample);
DECLARE_SHADER(BrdfIntegrate);
DECLARE_SHADER(MakeFrame);
DECLARE_SHADER(Rotate);
DECLARE_SHADER(Scale);
DECLARE_SHADER(TweakGlobals);
DECLARE_FCV_SHADER(Cache);
DECLARE_SHADER(CacheM);
DECLARE_SHADER(CacheBrdf);
DECLARE_SHADER(Wireframe);
DECLARE_SHADER(ShadowMatte);
DECLARE_SHADER(MakeRay);
DECLARE_SHADER(ReflectRay);
DECLARE_SHADER(RefractRay);
DECLARE_SHADER(RayStateI);
DECLARE_SHADER(RayStateF);
DECLARE_SHADER(RayStateV);
DECLARE_SHADER(Trace);
DECLARE_SHADER(TraceResultF);
DECLARE_SHADER(TraceResultC3);
DECLARE_SHADER(TraceResultV);
DECLARE_SHADER(ProbeResultV);
DECLARE_SHADER(Sampler);
DECLARE_SHADER(SampleValue);
DECLARE_FCV_SHADER(SampleLoop);
DECLARE_SHADER(SssSingle);
DECLARE_SHADER(SssMulti);
DECLARE_SHADER(GetNode);
DECLARE_SHADER(CompareN);
DECLARE_SHADER(BrdfStretchedPhong);
DECLARE_SHADER(BtdfMicrofacetIntegrate);
DECLARE_SHADER(CacheRay);
DECLARE_SHADER(SetOpacity);
DECLARE_SHADER(BrdfEvalSample);
DECLARE_SHADER(BrdfEvalPdf);
DECLARE_SHADER(BrdfEval);
DECLARE_FCV_SHADER(Lerp);
DECLARE_SHADER(SetRayDir);
DECLARE_SHADER(IsValidRay);
DECLARE_SHADER(IsTirRay);
DECLARE_SHADER(SampleDir);
DECLARE_SHADER(MicrofacetNormal);
DECLARE_SHADER(SwitchRayF);
DECLARE_SHADER(SwitchRayC3);
DECLARE_SHADER(SwitchRayC4);
#ifdef HAS_VOLUME
DECLARE_SHADER(Volume);
#endif
#ifdef ARNOLD_4_2_AND_ABOVE
DECLARE_SHADER(SampleVolumeF);
DECLARE_SHADER(SampleVolumeP2);
DECLARE_SHADER(SampleVolumeP3);
DECLARE_SHADER(SampleVolumeV);
DECLARE_SHADER(SampleVolumeC3);
DECLARE_SHADER(SampleVolumeC4);
#endif
#ifdef HAS_OBJECT_MATTE
DECLARE_SHADER(IsMatte);
#endif
DECLARE_SHADER(MakeF);
DECLARE_SHADER(C3ToC4);
DECLARE_SHADER(Gamma);
DECLARE_SHADER(AdaptChromaticity);

node_loader
{
   if (i >= gsShaderCount)
   {
      return false;
   }
   REGISTER_FCV_SHADER(Abs, abs)
   REGISTER_FCV_SHADER(Add, add)
   REGISTER_FCV_SHADER(Bias, bias)
   REGISTER_FCV_SHADER(BiasGain, bias_gain)
   REGISTER_FCV_SHADER(Ceil, ceil)
   REGISTER_FCV_SHADER(Clamp, clamp)
   REGISTER_SHADER(CompareF, compare_f, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(CompareC3, compare_c3, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(CompareV, compare_v, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(FToB, f_to_b, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(FToI, f_to_i, AI_TYPE_INT)
   REGISTER_SHADER(VToC3, v_to_c3, AI_TYPE_RGB)
   REGISTER_SHADER(C3ToV, c3_to_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(C3ToF, c3_to_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(FToV, f_to_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(FToC3, f_to_c3, AI_TYPE_RGB)
   REGISTER_SHADER(FToC4, f_to_c4, AI_TYPE_RGBA)
   REGISTER_SHADER(BToI, b_to_i, AI_TYPE_INT)
   REGISTER_SHADER(BToF, b_to_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(Cross, cross, AI_TYPE_VECTOR)
   REGISTER_FCV_SHADER(Gain, gain)
   REGISTER_FCV_SHADER(Div, div)
   REGISTER_SHADER(Dot, dot, AI_TYPE_FLOAT)
   REGISTER_FCV_SHADER(Exp, exp)
   REGISTER_FCV_SHADER(Floor, floor)
   REGISTER_FCV_SHADER(Inv, inv)
   REGISTER_FCV_SHADER(Log, log)
   REGISTER_FCV_SHADER(Max, max)
   REGISTER_FCV_SHADER(Min, min)
   REGISTER_FCV_SHADER(Mix, mix)
   REGISTER_FCV_SHADER(Mod, mod)
   REGISTER_FCV_SHADER(MultiSwitch, multi_switch)
   REGISTER_FCV_SHADER(Mult, mult)
   REGISTER_FCV_SHADER(Neg, neg)
   REGISTER_SHADER(Normalize, normalize, AI_TYPE_VECTOR)
   REGISTER_FCV_SHADER(Pow, pow)
   REGISTER_FCV_SHADER(Remap, remap)
   REGISTER_FCV_SHADER(Round, round)
   REGISTER_FCV_SHADER(Switch, switch)
   REGISTER_FCV_SHADER(Sign, sign)
   REGISTER_FCV_SHADER(SmoothStep, smooth_step)
   REGISTER_FCV_SHADER(Sqrt, sqrt)
   REGISTER_FCV_SHADER(Sub, sub)
   REGISTER_FLOAT_SHADER(Trigo, trigo)
   REGISTER_VECTOR_SHADER(Trigo, trigo)
   REGISTER_SHADER(Length, length, AI_TYPE_FLOAT)
   REGISTER_SHADER(ShapeAttrB, shape_attr_b, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(ShapeAttrI, shape_attr_i, AI_TYPE_INT)
   REGISTER_SHADER(ShapeAttrF, shape_attr_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(ShapeAttrP2, shape_attr_p2, AI_TYPE_POINT2)
   REGISTER_SHADER(ShapeAttrV, shape_attr_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(ShapeAttrC3, shape_attr_c3, AI_TYPE_RGB)
   REGISTER_SHADER(ShapeAttrC4, shape_attr_c4, AI_TYPE_RGBA)
   REGISTER_SHADER(ShapeAttrS, shape_attr_s, AI_TYPE_STRING)
#ifdef USE_AGSTATE
   REGISTER_SHADER(agIntState, globals_i, AI_TYPE_INT)
   REGISTER_SHADER(agFloatState, globals_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(agColorState, globals_c3, AI_TYPE_RGB)
   REGISTER_SHADER(agVectorState, globals_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(agMatrixState, globals_m, AI_TYPE_MATRIX)
   REGISTER_SHADER(agNodeState, globals_n, AI_TYPE_NODE)
#endif
   REGISTER_SHADER(Transform, transform, AI_TYPE_VECTOR)
   REGISTER_SHADER(MakeM, make_m, AI_TYPE_MATRIX)
   REGISTER_SHADER(MultM, mult_m, AI_TYPE_MATRIX)
   REGISTER_SHADER(SetTraceSet, set_trace_set, AI_TYPE_RGBA)
   REGISTER_SHADER(Shade, shade, AI_TYPE_RGB)
   REGISTER_SHADER(Holdout, holdout, AI_TYPE_RGBA)
   REGISTER_SHADER(ShadingPoint, shading_point, AI_TYPE_POINT)
   REGISTER_SHADER(ShadingNormal, shading_normal, AI_TYPE_VECTOR)
   REGISTER_SHADER(WriteAOVs, write_aovs, AI_TYPE_RGBA)
   REGISTER_SHADER(ReadAOVB, read_aov_b, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(ReadAOVI, read_aov_i, AI_TYPE_INT)
   REGISTER_SHADER(ReadAOVF, read_aov_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(ReadAOVP2, read_aov_p2, AI_TYPE_POINT2)
   REGISTER_SHADER(ReadAOVP3, read_aov_p3, AI_TYPE_POINT)
   REGISTER_SHADER(ReadAOVV, read_aov_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(ReadAOVC3, read_aov_c3, AI_TYPE_RGB)
   REGISTER_SHADER(ReadAOVC4, read_aov_c4, AI_TYPE_RGBA)
   REGISTER_SHADER(InvM, inv_m, AI_TYPE_MATRIX)
   REGISTER_SHADER(Transpose, transpose, AI_TYPE_MATRIX)
   REGISTER_SHADER(LerpM, lerp_m, AI_TYPE_MATRIX)
   REGISTER_FCV_SHADER(Ramp, ramp)
   REGISTER_FCV_SHADER(SmoothSwitch, smooth_switch)
   REGISTER_FCV_SHADER(RangeSwitch, range_switch)
   REGISTER_FCV_SHADER(SmoothRangeSwitch, smooth_range_switch)
   REGISTER_SHADER(MakeP2, make_p2, AI_TYPE_POINT2)
   REGISTER_SHADER(MakeV, make_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(MakeC3, make_c3, AI_TYPE_RGB)
   REGISTER_SHADER(MakeC4, make_c4, AI_TYPE_RGBA)
   REGISTER_FCV_SHADER(Gaussian, gaussian)
#ifdef USE_AGNOISES
   REGISTER_SHADER(Fractal, fractal_noise, AI_TYPE_FLOAT);
   REGISTER_SHADER(Voronoi, voronoi_noise, AI_TYPE_FLOAT);
   REGISTER_SHADER(DistortPoint, distort_point, AI_TYPE_POINT);
#endif
#ifdef USE_AGSEEXPR
   REGISTER_SHADER(agSeExpr, seexpr, AI_TYPE_VECTOR)
#endif
#ifdef USE_AGANIMCURVE
   REGISTER_SHADER(agAnimCurve, curve, AI_TYPE_FLOAT)
#endif
#ifdef USE_AGUSERDATARAMP
   REGISTER_SHADER(agUserDataFloatRamp, shape_attr_ramp_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(agUserDataColorRamp, shape_attr_ramp_c3, AI_TYPE_RGB)
   REGISTER_SHADER(agUserDataVectorRamp, shape_attr_ramp_v, AI_TYPE_VECTOR)
#endif
   REGISTER_SHADER(UVSwitch, uv_switch, AI_TYPE_RGBA)
   REGISTER_SHADER(UVProject, uv_project, AI_TYPE_RGBA)
   REGISTER_SHADER(UVTransform, uv_transform, AI_TYPE_RGBA)
   REGISTER_SHADER(UVClip, uv_clip, AI_TYPE_RGBA)
   REGISTER_SHADER(MakeTx, make_tx, AI_TYPE_STRING)
   REGISTER_SHADER(CameraTransform, camera_transform, AI_TYPE_MATRIX)
   REGISTER_SHADER(Resolution, resolution, AI_TYPE_VECTOR)
   REGISTER_SHADER(TokenPath, token_path, AI_TYPE_STRING)
   REGISTER_SHADER(Texture, texture, AI_TYPE_RGBA)
   REGISTER_SHADER(WriteMsgs, write_msgs, AI_TYPE_RGBA)
   REGISTER_SHADER(ReadMsgB, read_msg_b, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(ReadMsgI, read_msg_i, AI_TYPE_INT)
   REGISTER_SHADER(ReadMsgF, read_msg_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(ReadMsgP2, read_msg_p2, AI_TYPE_POINT2)
   REGISTER_SHADER(ReadMsgP3, read_msg_p3, AI_TYPE_POINT)
   REGISTER_SHADER(ReadMsgV, read_msg_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(ReadMsgC3, read_msg_c3, AI_TYPE_RGB)
   REGISTER_SHADER(ReadMsgC4, read_msg_c4, AI_TYPE_RGBA)
   REGISTER_SHADER(IlluminanceLoop, illuminance_loop, AI_TYPE_RGB)
   REGISTER_SHADER(LightWeight, light_weight, AI_TYPE_FLOAT)
   REGISTER_SHADER(ReflectV, reflect_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(RefractV, refract_v, AI_TYPE_VECTOR)
   REGISTER_FLOAT_SHADER(Fresnel, fresnel)
   REGISTER_COLOR_SHADER(Fresnel, fresnel)
   REGISTER_SHADER(BrdfOrenNayar, brdf_oren_nayar, AI_TYPE_RGB)
   REGISTER_SHADER(BrdfCookTorrance, brdf_cook_torrance, AI_TYPE_RGB)
   REGISTER_SHADER(BrdfWardDuer, brdf_ward_duer, AI_TYPE_RGB)
   REGISTER_SHADER(BrdfAshikhminShirley, brdf_ashikhmin_shirley, AI_TYPE_RGB)
   REGISTER_SHADER(EvalLightSample, eval_light_sample, AI_TYPE_RGB)
   REGISTER_SHADER(BrdfIntegrate, brdf_integrate, AI_TYPE_RGB)
   REGISTER_SHADER(MakeFrame, make_frame, AI_TYPE_MATRIX)
   REGISTER_SHADER(Rotate, rotate, AI_TYPE_VECTOR)
   REGISTER_SHADER(Scale, scale, AI_TYPE_VECTOR)
   REGISTER_SHADER(TweakGlobals, tweak_globals, AI_TYPE_RGB)
   REGISTER_FCV_SHADER(Cache, cache)
   REGISTER_SHADER(CacheM, cache_m, AI_TYPE_MATRIX)
   REGISTER_SHADER(CacheBrdf, cache_brdf, AI_TYPE_RGB)
   REGISTER_SHADER(Wireframe, wireframe, AI_TYPE_FLOAT)
   REGISTER_SHADER(ShadowMatte, shadow_matte, AI_TYPE_RGB)
   REGISTER_SHADER(MakeRay, make_ray, AI_TYPE_VECTOR)
   REGISTER_SHADER(ReflectRay, reflect_ray, AI_TYPE_VECTOR)
   REGISTER_SHADER(RefractRay, refract_ray, AI_TYPE_VECTOR)
   REGISTER_SHADER(RayStateI, ray_state_i, AI_TYPE_INT)
   REGISTER_SHADER(RayStateF, ray_state_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(RayStateV, ray_state_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(Trace, trace, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(TraceResultF, trace_result_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(TraceResultC3, trace_result_c3, AI_TYPE_RGB)
   REGISTER_SHADER(TraceResultV, trace_result_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(ProbeResultV, probe_result_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(Sampler, sampler, AI_TYPE_RGB)
   REGISTER_SHADER(SampleValue, sample_value, AI_TYPE_POINT2)
   REGISTER_FCV_SHADER(SampleLoop, sample_loop)
   REGISTER_SHADER(SssSingle, sss_single, AI_TYPE_RGB)
   REGISTER_SHADER(SssMulti, sss_multi, AI_TYPE_RGB)
   REGISTER_SHADER(GetNode, get_node, AI_TYPE_NODE)
   REGISTER_SHADER(CompareN, compare_n, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(BrdfStretchedPhong, brdf_stretched_phong, AI_TYPE_RGB)
   REGISTER_SHADER(BtdfMicrofacetIntegrate, btdf_microfacet_integrate, AI_TYPE_RGB)
   REGISTER_SHADER(CacheRay, cache_ray, AI_TYPE_VECTOR)
   REGISTER_SHADER(SetOpacity, set_opacity, AI_TYPE_RGB)
   REGISTER_SHADER(BrdfEvalSample, brdf_eval_sample, AI_TYPE_VECTOR)
   REGISTER_SHADER(BrdfEvalPdf, brdf_eval_pdf, AI_TYPE_FLOAT)
   REGISTER_SHADER(BrdfEval, brdf_eval, AI_TYPE_RGB)
   REGISTER_FCV_SHADER(Lerp, lerp)
   REGISTER_SHADER(SetRayDir, set_ray_dir, AI_TYPE_VECTOR)
   REGISTER_SHADER(IsValidRay, is_valid_ray, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(IsTirRay, is_tir_ray, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(SampleDir, sample_dir, AI_TYPE_VECTOR)
   REGISTER_SHADER(MicrofacetNormal, microfacet_normal, AI_TYPE_VECTOR)
   REGISTER_SHADER(SwitchRayF, switch_ray_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(SwitchRayC3, switch_ray_c3, AI_TYPE_RGB)
   REGISTER_SHADER(SwitchRayC4, switch_ray_c4, AI_TYPE_RGBA)
#ifdef HAS_VOLUME
   REGISTER_SHADER(Volume, volume, AI_TYPE_RGB)
#endif
#ifdef ARNOLD_4_2_AND_ABOVE
   REGISTER_SHADER(SampleVolumeF, sample_volume_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(SampleVolumeP2, sample_volume_p2, AI_TYPE_POINT2)
   REGISTER_SHADER(SampleVolumeP3, sample_volume_p3, AI_TYPE_POINT)
   REGISTER_SHADER(SampleVolumeV, sample_volume_v, AI_TYPE_VECTOR)
   REGISTER_SHADER(SampleVolumeC3, sample_volume_c3, AI_TYPE_RGB)
   REGISTER_SHADER(SampleVolumeC4, sample_volume_c4, AI_TYPE_RGBA)
#endif
#ifdef HAS_OBJECT_MATTE
   REGISTER_SHADER(IsMatte, is_matte, AI_TYPE_BOOLEAN)
#endif
   REGISTER_SHADER(MakeF, make_f, AI_TYPE_FLOAT)
   REGISTER_SHADER(C3ToC4, c3_to_c4, AI_TYPE_RGBA)
   REGISTER_SHADER(Gamma, gamma, AI_TYPE_RGB)
   REGISTER_SHADER(AdaptChromaticity, adapt_chromaticity, AI_TYPE_RGB);
   return false;
}
