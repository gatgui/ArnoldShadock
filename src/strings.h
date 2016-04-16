#ifndef __agShadingBlocks_strings_h__
#define __agShadingBlocks_strings_h__

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
}

#endif
