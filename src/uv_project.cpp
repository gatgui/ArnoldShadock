#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVProjectMtd);

enum UVProjectParams
{
   p_input = 0,
   p_lookup_vector,
   p_lookup_vector_space,
   p_mode,
   p_custom_vector,
   p_custom_is_point,
   p_custom_vector_ddx,
   p_custom_vector_ddy,
   p_offset_matrix,
   p_recompute_surface_uv_derivs
};

enum LookupVector
{
   LV_P = 0,
   LV_N
};

static const char* LookupVectorNames[] =
{
   "P",
   "N",
   NULL
};

enum LookupVectorSpace
{
   LVS_world = 0,
   LVS_object
};

static const char* LookupVectorSpaceNames[] =
{
   "world",
   "object",
   NULL
};

enum UVMode
{
   UV_planar = 0,
   UV_triplanar,
   UV_perspective,
   UV_angular_map,
   UV_cubic_map,
   UV_lat_long,
   UV_mirrored_ball
};

static const char* UVModeNames[] =
{
   "planar",
   "triplanar",
   "perspective",
   "angular_map",
   "cubic_map",
   "lat_long",
   "mirrored_ball",
   NULL
};

// ---

struct UVProjectData
{
   LookupVector lookup_vector;
   LookupVectorSpace space;
   bool use_custom_vector;
   bool custom_is_point;
   //bool use_custom_space;
   UVMode mode;
   bool recompute_surface_uv_derivs;
};

node_parameters
{
   AtMatrix id;
   
   AiM4Identity(id);
   
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterEnum("lookup_vector", LV_N, LookupVectorNames);
   AiParameterEnum("lookup_vector_space", LVS_world, LookupVectorSpaceNames);
   AiParameterEnum("mode", UV_cubic_map, UVModeNames);
   AiParameterVec("custom_vector", 0.0f, 0.0f, 1.0f);
   AiParameterBool("custom_is_point", false);
   AiParameterVec("custom_vector_ddx", 0.0f, 0.0f, 0.0f);
   AiParameterVec("custom_vector_ddy", 0.0f, 0.0f, 0.0f);
   AiParameterMtx("offset_matrix", id);
   AiParameterBool("recompute_surface_uv_derivs", false);
   
   AiMetaDataSetBool(mds, "lookup_vector", "linkable", false);
   AiMetaDataSetBool(mds, "lookup_vector_space", "linkable", false);
   AiMetaDataSetBool(mds, "custom_is_point", "linkable", false);
   AiMetaDataSetBool(mds, "mode", "linkable", false);
   AiMetaDataSetBool(mds, "recompute_surface_uv_derivs", "linkable", false);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(UVProjectData)));
}

node_update
{
   UVProjectData *data = (UVProjectData*) AiNodeGetLocalData(node);
   
   data->lookup_vector = (LookupVector) AiNodeGetInt(node, "lookup_vector");
   data->space = (LookupVectorSpace) AiNodeGetInt(node, "lookup_vector_space");
   data->use_custom_vector = AiNodeIsLinked(node, "custom_vector");
   data->custom_is_point = AiNodeGetBool(node, "custom_is_point");
   //data->use_custom_space = AiNodeIsLinked(node, "custom_space");
   data->mode = (UVMode) AiNodeGetInt(node, "mode");
   data->recompute_surface_uv_derivs = AiNodeGetBool(node, "recompute_surface_uv_derivs");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   AtVector dPdx, dPdy, v0, v1, v2;
   AtHPoint d, ddx, ddy, h;
   AtMatrix M, *Moff = 0;
   bool withDerivatives = false;
   
   UVData uvs(sg);
   
   UVProjectData *data = (UVProjectData*) AiNodeGetLocalData(node);
   
   if (data->recompute_surface_uv_derivs)
   {
      // Compute acutal dPdx and dPdv from dudx, dudy, dvdx and dvdy
      ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   }
   
   Moff = AiShaderEvalParamMtx(p_offset_matrix);
   
   if (data->use_custom_vector)
   {
      v0 = AiShaderEvalParamVec(p_custom_vector);
      AiV4Create(d, v0.x, v0.y, v0.y, (data->custom_is_point ? 1.0f : 0.0f));
      
      v1 = AiShaderEvalParamVec(p_custom_vector_ddx);
      
      v2 = AiShaderEvalParamVec(p_custom_vector_ddy);
      
      if (!AiV3IsZero(v1) && !AiV3IsZero(v2))
      {
         withDerivatives = true;
         
         AiV4Create(ddx, d.x + v1.x, d.y + v1.y, d.z + v1.z, d.w);
         AiV4Create(ddy, d.x + v2.x, d.y + v2.y, d.z + v2.z, d.w);
      }
      
      AiM4Copy(M, *Moff);
   }
   else
   {
      withDerivatives = true;
      
      if (data->lookup_vector == LV_P)
      {
         AiV4CreatePoint(d, sg->P);
         AiV4Create(ddx, sg->P.x + sg->dPdx.x, sg->P.y + sg->dPdx.y, sg->P.z + sg->dPdx.z, 1.0f);
         AiV4Create(ddy, sg->P.x + sg->dPdy.x, sg->P.y + sg->dPdy.y, sg->P.z + sg->dPdy.z, 1.0f);
      }
      else
      {
         AiV4CreateVector(d, sg->N);
         #ifdef ARNOLD_4_1_AND_ABOVE
         AiV4Create(ddx, sg->N.x + sg->dNdx.x, sg->N.y + sg->dNdx.y, sg->N.z + sg->dNdx.z, 0.0f);
         AiV4Create(ddy, sg->N.x + sg->dNdy.x, sg->N.y + sg->dNdy.y, sg->N.z + sg->dNdy.z, 0.0f);
         #else
         withDerivatives = false;
         #endif
      }
      
      if (data->space == LVS_object)
      {
         AiM4Mult(M, sg->Minv, *Moff);
      }
      else
      {
         AiM4Copy(M, *Moff);
      }
   }
   
   AiM4HPointByMatrixMult(&h, M, &d);
   d = h;
   AiM4HPointByMatrixMult(&h, M, &ddx);
   ddx = h;
   AiM4HPointByMatrixMult(&h, M, &ddy);
   ddy = h;
   
   switch (data->mode)
   {
   case UV_triplanar:
      {
         float ax = d.x > 0.0f ? d.x : -d.x;
         float ay = d.y > 0.0f ? d.y : -d.y;
         float az = d.z > 0.0f ? d.z : -d.z;
         
         if (ax > ay && ax > az)
         {
            // yz plane
            sg->u = 0.5f * (d.z + 1.0f);
            sg->v = 0.5f * (d.y + 1.0f);
            if (withDerivatives)
            {
               sg->dudx = 0.5f * (ddx.z + 1.0f) - sg->u;
               sg->dvdx = 0.5f * (ddx.y + 1.0f) - sg->v;
               sg->dudy = 0.5f * (ddy.z + 1.0f) - sg->u;
               sg->dvdy = 0.5f * (ddy.y + 1.0f) - sg->v;
            }
         }
         else if (ay > ax && ay > az)
         {
            // xz plane
            sg->u = 0.5f * (d.x + 1.0f);
            sg->v = 0.5f * (d.z + 1.0f);
            if (withDerivatives)
            {
               sg->dudx = 0.5f * (ddx.x + 1.0f) - sg->u;
               sg->dvdx = 0.5f * (ddx.z + 1.0f) - sg->v;
               sg->dudy = 0.5f * (ddy.x + 1.0f) - sg->u;
               sg->dvdy = 0.5f * (ddy.z + 1.0f) - sg->v;
            }
         }
         else
         {
            // xy plane
            sg->u = 0.5f * (d.x + 1.0f);
            sg->v = 0.5f * (d.y + 1.0f);
            if (withDerivatives)
            {
               sg->dudx = 0.5f * (ddx.x + 1.0f) - sg->u;
               sg->dvdx = 0.5f * (ddx.y + 1.0f) - sg->v;
               sg->dudy = 0.5f * (ddy.x + 1.0f) - sg->u;
               sg->dvdy = 0.5f * (ddy.y + 1.0f) - sg->v;
            }
         }
      }
      break;
   case UV_perspective:
      {
         float inv = 1.0f / d.w;
         
         d.x *= inv;
         d.y *= inv;
         
         if (withDerivatives)
         {
            inv = 1.0f / ddx.w;
            ddx.x *= inv;
            ddx.y *= inv;
            
            inv = 1.0f / ddy.w;
            ddy.x *= inv;
            ddy.y *= inv;
         }
      }
   case UV_planar:
      // Common to UV_planar and UV_perspective
      sg->u = 0.5f * (d.x + 1.0f);
      sg->v = 0.5f * (d.y + 1.0f);
      if (withDerivatives)
      {
         sg->dudx = 0.5f * (ddx.x + 1.0f) - sg->u;
         sg->dvdx = 0.5f * (ddx.y + 1.0f) - sg->v;
         sg->dudy = 0.5f * (ddy.x + 1.0f) - sg->u;
         sg->dvdy = 0.5f * (ddy.y + 1.0f) - sg->v;
      }
      break;
   case UV_angular_map:
      AiV3Create(v0, d.x, d.y, d.z);
      if (withDerivatives)
      {
         AiV3Create(v1, ddx.x, ddx.y, ddx.z);
         AiV3Create(v2, ddy.x, ddy.y, ddy.z);
         v1 -= v0;
         v2 -= v0;
         AiMappingAngularMapDerivs(&v0, &v1, &v2, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
      }
      else
      {
         AiMappingAngularMap(&v0, &(sg->u), &(sg->v));
      }
      break;
   case UV_cubic_map:
      AiV3Create(v0, d.x, d.y, d.z);
      if (withDerivatives)
      {
         AiV3Create(v1, ddx.x, ddx.y, ddx.z);
         AiV3Create(v2, ddy.x, ddy.y, ddy.z);
         v1 -= v0;
         v2 -= v0;
         AiMappingCubicMapDerivs(&v0, &v1, &v2, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
      }
      else
      {
         AiMappingCubicMap(&v0, &(sg->u), &(sg->v));
      }
      break;
   case UV_lat_long:
      AiV3Create(v0, d.x, d.y, d.z);
      if (withDerivatives)
      {
         AiV3Create(v1, ddx.x, ddx.y, ddx.z);
         AiV3Create(v2, ddy.x, ddy.y, ddy.z);
         v1 -= v0;
         v2 -= v0;
         AiMappingLatLongDerivs(&v0, &v1, &v2, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
      }
      else
      {
         AiMappingLatLong(&v0, &(sg->u), &(sg->v));
      }
      break;
   case UV_mirrored_ball:
      AiV3Create(v0, d.x, d.y, d.z);
      if (withDerivatives)
      {
         AiV3Create(v1, ddx.x, ddx.y, ddx.z);
         AiV3Create(v2, ddy.x, ddy.y, ddy.z);
         v1 -= v0;
         v2 -= v0;
         AiMappingMirroredBallDerivs(&v0, &v1, &v2, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
      }
      else
      {
         AiMappingMirroredBall(&v0, &(sg->u), &(sg->v));
      }
      break;
   default:
      break;
   };
   
   if (!withDerivatives)
   {
      sg->dudx = 0.0f;
      sg->dudy = 0.0f;
      sg->dvdx = 0.0f;
      sg->dvdy = 0.0f;
   }
   else if (data->recompute_surface_uv_derivs)
   {
      // Re-compute dPdu and dPdv from dPdx, dPdy, dudx, dudy, dvdx, dvdy (invariant)
      ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
   }
   
   sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   
   uvs.restore(sg);
}
