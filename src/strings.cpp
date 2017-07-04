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

#include "strings.h"

namespace SSTR
{
#ifndef WITH_USERDATARAMP_SHADERS
   AtString linkable("linkable");
#endif
   AtString source("source");
   AtString source_color_space("source_color_space");
   AtString target("target");
   AtString target_color_space("target_color_space");
   AtString transform("transform");
   
   AtString temperature("temperature");
   AtString output_mode("output_mode");
   AtString tm_key("tm_key");
   AtString tm_Lavg("tm_Lavg");
   AtString tm_Lwht("tm_Lwht");
   AtString ca_transform("ca_transform");
   AtString ca_max_temperature("ca_max_temperature");
   AtString ca_white_point("ca_white_point");
   AtString exposure("exposure");
   AtString color_space("color_space");
   AtString fix_nans("fix_nans");
   AtString nan_color("nan_color");
   
   AtString roughness_x("roughness_x");
   AtString roughness_y("roughness_y");
   AtString glossiness_x("glossiness_x");
   AtString glossiness_y("glossiness_y");
   AtString local_frame("local_frame");
   AtString custom_frame("custom_frame");
   AtString frame_rotation("frame_rotation");
   AtString angle_units("angle_units");
   
   AtString distribution("distribution");
   AtString dispersion("dispersion");
   AtString fresnel("fresnel");
   AtString weight("weight");
   AtString ior("ior");
   AtString transmittance("transmittance");
   AtString ray_type("ray_type");
   AtString distribution_param("distribution_param");
   
   AtString mode("mode");
   AtString camera("camera");
   AtString camera_name("camera_name");
   
   AtString _operator("operator");
   
   AtString surface_color("surface_color");
   AtString surface_color_weight("surface_color_weight");
   AtString light_weight("light_weight");
   
   AtString combine("combine");
   AtString reset_lights_cache("reset_lights_cache");
   
   AtString view_vector("view_vector");
   AtString normal("normal");
   AtString reflectance("reflectance");
   
   AtString logc_exposure("logc_exposure");
   
   AtString target_node("target_node");
   AtString light_index("light_index");
   
   AtString agsb_brdf("agsb_brdf");
   AtString agsb_ray("agsb_ray");
   AtString agsb_trace_hit("agsb_trace_hit");
   AtString agsb_tir("agsb_tir");
   AtString agsb_sampler("agsb_sampler");
   AtString agsb_sample_value("agsb_sample_value");
   
   AtString method("method");
   AtString origin("origin");
   AtString custom_origin("custom_origin");
   AtString z_axis("z_axis");
   AtString custom_axis("custom_axis");
   
   AtString transform_order("transform_order");
   AtString translation("translation");
   AtString rotation_order("rotation_order");
   AtString rotation("rotation");
   AtString rotation_pivot("rotation_pivot");
   AtString scale("scale");
   AtString scale_pivot("scale_pivot");
   
   AtString filename("filename");
   AtString format("format");
   AtString wraps("wraps");
   AtString wrapt("wrapt");
   AtString tile("tile");
   AtString resize("resize");
   AtString mipmap("mipmap");
   AtString filter("filter");
   AtString stripxmp("stripxmp");
   AtString oiioopt("oiioopt");
   AtString filepath("filepath");
   AtString threads("threads");
   
   AtString state("state");
   
#ifndef WITH_USERDATARAMP_SHADERS
   AtString positions("positions");
   AtString values("values");
   AtString interpolations("interpolations");
#endif

   AtString aov_name("aov_name");
   
   AtString msg_name("msg_name");
   
   AtString direction("direction");
   AtString safe("safe");
   AtString geometric_normal("geometric_normal");
   
   AtString type("type");
   
   AtString ior1("ior1");
   AtString ior2("ior2");
   
   AtString xres("xres");
   AtString yres("yres");
   
   AtString rotation_dot_x("rotation.x");
   AtString rotation_dot_y("rotation.y");
   AtString rotation_dot_z("rotation.z");
   
   AtString samples("samples");
   AtString seed("seed");
   AtString seeded("seeded");
   
   AtString field("field");
   AtString interpolation("interpolation");
   AtString _default("default");
   
   AtString trace_set("trace_set");
   AtString inclusive("inclusive");
   
   AtString override_normal("override_normal");

   AtString time("time");
   AtString faceforward("faceforward");
   AtString space("space");
   AtString custom_space("custom_space");

   AtString attribute("attribute");

   AtString enable_attr("enable_attr");
   AtString color_attr("color_attr");
   AtString default_enable("default_enable");
   AtString default_color("default_color");

   AtString camera_use_default("camera_use_default");
   AtString shadow("shadow");
   AtString shadow_use_default("shadow_use_default");
   AtString reflected("reflected");
   AtString reflected_use_default("reflected_use_default");
   AtString refracted("refracted");
   AtString refracted_use_default("refracted_use_default");
   AtString subsurface("subsurface");
   AtString subsurface_use_default("subsurface_use_default");
   AtString diffuse("diffuse");
   AtString diffuse_use_default("diffuse_use_default");
   AtString glossy("glossy");
   AtString glossy_use_default("glossy_use_default");

   AtString mipmap_mode("mipmap_mode");
   AtString mipmap_bias("mipmap_bias");
   AtString single_channel("single_channel");
   AtString fill("fill");
   AtString swrap("swrap");
   AtString twrap("twrap");
   AtString sflip("sflip");
   AtString tflip("tflip");
   AtString swidth("swidth");
   AtString twidth("twidth");
   AtString sblur("sblur");
   AtString tblur("tblur");
   AtString swap_st("swap_st");
   AtString cache_texture_handles("cache_texture_handles");
   AtString use_default_color("use_default_color");

   AtString frame("frame");

   AtString matrix("matrix");
   AtString as_point("as_point");

   AtString operation("operation");

   AtString P("P");
   AtString P_space("P_space");
   AtString P_world_matrix("P_world_matrix");
   AtString P_is_offset("P_is_offset");
   AtString N("N");
   AtString N_space("N_space");
   AtString N_world_matrix("N_world_matrix");
   AtString fhemi("fhemi");
   AtString skip_shadow("skip_shadow");
   AtString inclusive_trace_set("inclusive_trace_set");

   AtString lookup_vector("lookup_vector");
   AtString lookup_vector_space("lookup_vector_space");
   AtString custom_vector("custom_vector");
   AtString custom_is_point("custom_is_point");

   AtString uv_set_name("uv_set_name");

   AtString order("order");
   AtString transform_pivots("transform_pivots");
   AtString recompute_surface_uv_derivs("recompute_surface_uv_derivs");

   AtString units("units");
   AtString edge_type("edge_type");

   AtString bool_aov_name("bool_aov_name");
   AtString int_aov_name("int_aov_name");
   AtString float_aov_name("float_aov_name");
   AtString point2_aov_name("point2_aov_name");
   AtString point_aov_name("point_aov_name");
   AtString vector_aov_name("vector_aov_name");
   AtString rgb_aov_name("rgb_aov_name");
   AtString rgba_aov_name("rgba_aov_name");
   AtString blend_opacity("blend_opacity");

   AtString bool_msg_name("bool_msg_name");
   AtString int_msg_name("int_msg_name");
   AtString float_msg_name("float_msg_name");
   AtString point2_msg_name("point2_msg_name");
   AtString point_msg_name("point_msg_name");
   AtString vector_msg_name("vector_msg_name");
   AtString rgb_msg_name("rgb_msg_name");
   AtString rgba_msg_name("rgba_msg_name");

   AtString bias("bias");
   AtString gain("gain");
   AtString multiply("multiply");
   AtString offset("offset");

   AtString scattering_source("scattering_source");
   AtString scattering_field("scattering_field");
   AtString scattering("scattering");
   AtString scattering_weight("scattering_weight");
   AtString scattering_color("scattering_color");
   AtString anisotropy("anisotropy");
   AtString attenuation_source("attenuation_source");
   AtString attenuation_field("attenuation_field");
   AtString attenuation_mode("attenuation_mode");
   AtString attenuation("attenuation");
   AtString attenuation_weight("attenuation_weight");
   AtString attenuation_color("attenuation_color");
   AtString preserve_energy("preserve_energy");
   AtString emission_source("emission_source");
   AtString emission_field("emission_field");
   AtString emission("emission");
   AtString emission_weight("emission_weight");
   AtString emission_color("emission_color");

   AtString input("input");
   AtString motion_start_frame("motion_start_frame");
   AtString motion_end_frame("motion_end_frame");
   AtString relative_motion_frame("relative_motion_frame");
#ifndef WITH_USERDATARAMP_SHADERS
   AtString default_interpolation("default_interpolation");
#endif
   AtString shutter_start("shutter_start");
   AtString shutter_end("shutter_end");
   AtString fps("fps");

   AtString use_approximation("use_approximation");
   
   AtString aov_input("aov_input");
   AtString msg_input("msg_input");
   AtString eval_order("eval_order");

   AtString user_defined("user_defined");

   AtString input_min("input_min");
   AtString input_max("input_max");
   AtString output_min("output_min");
   AtString output_max("output_max");
   AtString clamp_min("clamp_min");
   AtString clamp_max("clamp_max");
   AtString pre_multiply("pre_multiply");
   AtString pre_offset("pre_offset");
   AtString blend_mode("blend_mode");
   AtString blend("blend");
   AtString blend_min("blend_min");
   AtString blend_max("blend_max");

#ifdef WITH_ANIMCURVE_SHADER
   AtString input_is_frame_offset("input_is_frame_offset");
   AtString motion_steps("motion_steps");
   AtString in_tangents("in_tangents");
   AtString in_weights("in_weights");
   AtString out_tangents("out_tangents");
   AtString out_weights("out_weights");
   AtString pre_infinity("pre_infinity");
   AtString post_infinity("post_infinity");
#endif

#ifdef WITH_NOISE_SHADERS
   AtString Pref("Pref");
   AtString distance_func("distance_func");
   AtString custom_input("custom_input");
   AtString base_noise("base_noise");
#endif

#ifdef WITH_STATE_SHADERS
   AtString index("index");
#endif

#ifdef WITH_SEEXPR_SHADER
   AtString expression("expression");
   AtString fparam_name("fparam_name");
   AtString fparam_value("fparam_value");
   AtString vparam_name("vparam_name");
   AtString vparam_value("vparam_value");
   AtString stop_on_error("stop_on_error");
#endif

   AtString in_white("in_white");
   AtString in_black("in_black");
   AtString out_white("out_white");
   AtString out_black("out_black");
   AtString saturation_scale("saturation_scale");
   AtString hue_shift("hue_shift");
   AtString gamma("gamma");
}
