#ifndef __shadock_strings_h__
#define __shadock_strings_h__

#include <ai.h>

namespace SSTR
{
   extern AtString linkable;
   extern AtString source;
   extern AtString source_color_space;
   extern AtString target;
   extern AtString target_color_space;
   extern AtString transform;
   
   extern AtString temperature;
   extern AtString output_mode;
   extern AtString tm_key;
   extern AtString tm_Lavg;
   extern AtString tm_Lwht;
   extern AtString ca_transform;
   extern AtString ca_max_temperature;
   extern AtString ca_white_point;
   extern AtString exposure;
   extern AtString color_space;
   extern AtString fix_nans;
   extern AtString nan_color;
   
   extern AtString roughness_x;
   extern AtString roughness_y;
   extern AtString glossiness_x;
   extern AtString glossiness_y;
   extern AtString local_frame;
   extern AtString custom_frame;
   extern AtString frame_rotation;
   extern AtString angle_units;
   
   extern AtString distribution;
   extern AtString dispersion;
   extern AtString fresnel;
   extern AtString weight;
   extern AtString ior;
   extern AtString transmittance;
   extern AtString ray_type;
   extern AtString distribution_param;
   
   extern AtString mode;
   extern AtString camera;
   extern AtString camera_name;
   
   extern AtString _operator;
   
   extern AtString surface_color;
   extern AtString surface_color_weight;
   extern AtString light_weight;
   
   extern AtString combine;
   extern AtString reset_lights_cache;
   
   extern AtString view_vector;
   extern AtString normal;
   extern AtString reflectance;
   
   extern AtString logc_exposure;
   
   extern AtString target_node;
   extern AtString light_index;
   
   extern AtString agsb_brdf;
   extern AtString agsb_ray;
   extern AtString agsb_trace_hit;
   extern AtString agsb_tir;
   extern AtString agsb_sampler;
   extern AtString agsb_sample_value;
   
   extern AtString method;
   extern AtString origin;
   extern AtString custom_origin;
   extern AtString z_axis;
   extern AtString custom_axis;
   
   extern AtString transform_order;
   extern AtString translation;
   extern AtString rotation_order;
   extern AtString rotation;
   extern AtString rotation_pivot;
   extern AtString scale;
   extern AtString scale_pivot;
   
   extern AtString filename;
   extern AtString format;
   extern AtString wraps;
   extern AtString wrapt;
   extern AtString tile;
   extern AtString resize;
   extern AtString mipmap;
   extern AtString filter;
   extern AtString stripxmp;
   extern AtString oiioopt;
   extern AtString filepath;
   extern AtString threads;

   extern AtString mipmap_mode;
   extern AtString mipmap_bias;
   extern AtString single_channel;
   extern AtString fill;
   extern AtString swrap;
   extern AtString twrap;
   extern AtString sflip;
   extern AtString tflip;
   extern AtString swidth;
   extern AtString twidth;
   extern AtString sblur;
   extern AtString tblur;
   extern AtString swap_st;
   extern AtString cache_texture_handles;
   extern AtString use_default_color;
   
   extern AtString frame;

   extern AtString state;
   
   extern AtString positions;
   extern AtString values;
   extern AtString interpolations;
   
   extern AtString aov_name;
   
   extern AtString msg_name;
   
   extern AtString direction;
   extern AtString safe;
   extern AtString geometric_normal;
   
   extern AtString type;
   
   extern AtString ior1;
   extern AtString ior2;
   
   extern AtString xres;
   extern AtString yres;
   
   extern AtString rotation_dot_x;
   extern AtString rotation_dot_y;
   extern AtString rotation_dot_z;
   
   extern AtString samples;
   extern AtString seed;
   extern AtString seeded;
   
   extern AtString field;
   extern AtString interpolation;
   extern AtString _default;
   
   extern AtString trace_set;
   extern AtString inclusive;
   
   extern AtString override_normal;

   extern AtString time;
   extern AtString faceforward;
   extern AtString space;
   extern AtString custom_space;

   extern AtString attribute;

   extern AtString enable_attr;
   extern AtString color_attr;
   extern AtString default_enable;
   extern AtString default_color;

   extern AtString camera_use_default;
   extern AtString shadow;
   extern AtString shadow_use_default;
   extern AtString reflected;
   extern AtString reflected_use_default;
   extern AtString refracted;
   extern AtString refracted_use_default;
   extern AtString subsurface;
   extern AtString subsurface_use_default;
   extern AtString diffuse;
   extern AtString diffuse_use_default;
   extern AtString glossy;
   extern AtString glossy_use_default;

   extern AtString matrix;
   extern AtString as_point;

   extern AtString operation;

   extern AtString P;
   extern AtString P_space;
   extern AtString P_world_matrix;
   extern AtString P_is_offset;
   extern AtString N;
   extern AtString N_space;
   extern AtString N_world_matrix;
   extern AtString fhemi;
   extern AtString skip_shadow;
   extern AtString inclusive_trace_set;

   extern AtString lookup_vector;
   extern AtString lookup_vector_space;
   extern AtString custom_vector;
   extern AtString custom_is_point;

   extern AtString uv_set_name;

   extern AtString order;
   extern AtString transform_pivots;
   extern AtString recompute_surface_uv_derivs;

   extern AtString units;
   extern AtString edge_type;

   extern AtString bool_aov_name;
   extern AtString int_aov_name;
   extern AtString float_aov_name;
   extern AtString point2_aov_name;
   extern AtString point_aov_name;
   extern AtString vector_aov_name;
   extern AtString rgb_aov_name;
   extern AtString rgba_aov_name;
   extern AtString blend_opacity;

   extern AtString bool_msg_name;
   extern AtString int_msg_name;
   extern AtString float_msg_name;
   extern AtString point2_msg_name;
   extern AtString point_msg_name;
   extern AtString vector_msg_name;
   extern AtString rgb_msg_name;
   extern AtString rgba_msg_name;

   extern AtString bias;
   extern AtString gain;
   extern AtString multiply;
   extern AtString offset;

   extern AtString scattering_source;
   extern AtString scattering_field;
   extern AtString scattering;
   extern AtString scattering_weight;
   extern AtString scattering_color;
   extern AtString anisotropy;
   extern AtString attenuation_source;
   extern AtString attenuation_field;
   extern AtString attenuation_mode;
   extern AtString attenuation;
   extern AtString attenuation_weight;
   extern AtString attenuation_color;
   extern AtString preserve_energy;
   extern AtString emission_source;
   extern AtString emission_field;
   extern AtString emission;
   extern AtString emission_weight;
   extern AtString emission_color;
   
   extern AtString use_approximation;
   
   extern AtString aov_input;
   extern AtString msg_input;
   extern AtString eval_order;
   
   extern AtString user_defined;
   
   extern AtString input_min;
   extern AtString input_max;
   extern AtString output_min;
   extern AtString output_max;
   extern AtString clamp_min;
   extern AtString clamp_max;
   extern AtString pre_multiply;
   extern AtString pre_offset;
   extern AtString blend_mode;
   extern AtString blend;
   extern AtString blend_min;
   extern AtString blend_max;
}

#endif
