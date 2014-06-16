#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVProjectMtd);

enum UVProjectParams
{
   p_input = 0,
   p_lookup_vector,
   p_lookup_vector_space,
   p_mode,
   p_custom_vector,
   p_custom_vector_ddx,
   p_custom_vector_ddy,
   p_custom_space,
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
   UV_angular_map = 0,
   UV_cubic_map ,
   UV_lat_long,
   UV_mirrored_ball
};

static const char* UVModeNames[] =
{
   "angular_map",
   "cubic_map",
   "lat_long",
   "mirrored_ball",
   NULL
};

// ---

struct NodeData
{
   LookupVector lookup_vector;
   LookupVectorSpace space;
   bool use_custom_vector;
   bool use_custom_space;
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
   AiParameterVec("custom_vector_ddx", 0.0f, 0.0f, 0.0f);
   AiParameterVec("custom_vector_ddy", 0.0f, 0.0f, 0.0f);
   AiParameterMtx("custom_space", id);
   AiParameterBool("recompute_surface_uv_derivs", false);
   
   AiMetaDataSetBool(mds, "lookup_vector", "linkable", false);
   AiMetaDataSetBool(mds, "lookup_vector_space", "linkable", false);
   AiMetaDataSetBool(mds, "mode", "linkable", false);
   AiMetaDataSetBool(mds, "recompute_surface_uv_derivs", "linkable", false);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->lookup_vector = (LookupVector) AiNodeGetInt(node, "lookup_vector");
   data->space = (LookupVectorSpace) AiNodeGetInt(node, "lookup_vector_space");
   data->use_custom_vector = AiNodeIsLinked(node, "custom_vector");
   data->use_custom_space = AiNodeIsLinked(node, "custom_space");
   data->mode = (UVMode) AiNodeGetInt(node, "mode");
   data->recompute_surface_uv_derivs = AiNodeGetBool(node, "recompute_surface_uv_derivs");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   AtVector dPdx, dPdy, d, ddx, ddy, tmp;
   
   UVData uvs(sg);
   
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   if (data->recompute_surface_uv_derivs)
   {
      // Compute acutal dPdx and dPdv from dudx, dudy, dvdx and dvdy
      ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   }
   
   if (data->use_custom_vector)
   {
      d = AiShaderEvalParamVec(p_custom_vector);
      ddx = AiShaderEvalParamVec(p_custom_vector_ddx);
      ddy = AiShaderEvalParamVec(p_custom_vector_ddy);
      // ignore custom_space
   }
   else
   {
      AtMatrix *S = 0;
      AtMatrix Sinv;
      
      if (data->use_custom_space)
      {
         S = AiShaderEvalParamMtx(p_custom_space);
         AiM4Invert(*S, Sinv);
      }
      
      if (data->lookup_vector == LV_P)
      {
         if (S)
         {
            tmp = sg->P;
            AiM4PointByMatrixMult(&d, Sinv, &tmp);
            tmp = sg->P + sg->dPdx;
            AiM4PointByMatrixMult(&ddx, Sinv, &tmp);
            tmp = sg->P + sg->dPdy;
            AiM4PointByMatrixMult(&ddy, Sinv, &tmp);
            AiV3Normalize(d);
            AiV3Normalize(ddx);
            AiV3Normalize(ddy);
            ddx -= d;
            ddy -= d;
         }
         else if (data->space == LVS_world)
         {
            d = sg->P;
            ddx = sg->P + sg->dPdx;
            ddy = sg->P + sg->dPdy;
            AiV3Normalize(d);
            AiV3Normalize(ddx);
            AiV3Normalize(ddy);
            ddx -= d;
            ddy -= d;
         }
         else
         {
            d = sg->Po;
            tmp = sg->P + sg->dPdx;
            AiM4PointByMatrixMult(&ddx, sg->Minv, &tmp);
            tmp = sg->P + sg->dPdy;
            AiM4PointByMatrixMult(&ddy, sg->Minv, &tmp);
            AiV3Normalize(d);
            AiV3Normalize(ddx);
            AiV3Normalize(ddy);
            ddx -= d;
            ddy -= d;
         }
      }
      else
      {
         if (S)
         {
            tmp = sg->N;
            AiM4VectorByMatrixMult(&d, Sinv, &tmp);
            tmp = sg->dNdx;
            AiM4VectorByMatrixMult(&ddx, Sinv, &tmp);
            tmp = sg->dNdy;
            AiM4VectorByMatrixMult(&ddy, Sinv, &tmp);
         }
         else if (data->space == LVS_world)
         {
            d = sg->N;
            ddx = sg->dNdx;
            ddy = sg->dNdy;
         }
         else
         {
            tmp = sg->N;
            AiM4VectorByMatrixMult(&d, sg->Minv, &tmp);
            tmp = sg->dNdx;
            AiM4VectorByMatrixMult(&ddx, sg->Minv, &tmp);
            tmp = sg->dNdy;
            AiM4VectorByMatrixMult(&ddx, sg->Minv, &tmp);
         }
      }
   }
   
   if (AiV3IsZero(ddx) && AiV3IsZero(ddy))
   {
      switch (data->mode)
      {
      case UV_angular_map:
         AiMappingAngularMap(&d, &(sg->u), &(sg->v));
         break;
      case UV_cubic_map:
         AiMappingCubicMap(&d, &(sg->u), &(sg->v));
         break;
      case UV_lat_long:
         AiMappingLatLong(&d, &(sg->u), &(sg->v));
         break;
      case UV_mirrored_ball:
         AiMappingMirroredBall(&d, &(sg->u), &(sg->v));
         break;
      default:
         break;
      };
      
      sg->dudx = 0.0f;
      sg->dudy = 0.0f;
      sg->dvdx = 0.0f;
      sg->dvdy = 0.0f;
   }
   else
   {
      switch (data->mode)
      {
      case UV_angular_map:
         AiMappingAngularMapDerivs(&d, &ddx, &ddy, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
         break;
      case UV_cubic_map:
         AiMappingCubicMapDerivs(&d, &ddx, &ddy, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
         break;
      case UV_lat_long:
         AiMappingLatLongDerivs(&d, &ddx, &ddy, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
         break;
      case UV_mirrored_ball:
         AiMappingMirroredBallDerivs(&d, &ddx, &ddy, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
         break;
      default:
         break;
      };
      
      if (data->recompute_surface_uv_derivs)
      {
         // Re-compute dPdu and dPdv from dPdx and dPdy (invariant)
         ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
      }
   }
   
   sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   
   uvs.restore(sg);
}
