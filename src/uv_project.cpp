#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVProjectMtd);

enum UVProjectParams
{
   p_input = 0,
   p_mode,
   p_lookup_vector,
   p_lookup_vector_ddx,
   p_lookup_vector_ddy,
   p_recompute_surface_uv_derivs
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
   UVMode mode;
   bool recompute_surface_uv_derivs;
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("mode", UV_cubic_map, UVModeNames);
   AiParameterVec("lookup_vector", 0.0f, 0.0f, 1.0f);
   AiParameterVec("lookup_vector_ddx", 0.0f, 0.0f, 0.0f);
   AiParameterVec("lookup_vector_ddy", 0.0f, 0.0f, 0.0f);
   AiParameterBool("recompute_surface_uv_derivs", false);
   
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
   
   data->mode = (UVMode) AiNodeGetInt(node, "mode");
   data->recompute_surface_uv_derivs = AiNodeGetBool(node, "recompute_surface_uv_derivs");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   AtVector dPdx, dPdy, d, ddx, ddy;
   
   UVData uvs(sg);
   
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   if (data->recompute_surface_uv_derivs)
   {
      // Compute acutal dPdx and dPdv from dudx, dudy, dvdx and dvdy
      ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   }
   
   d = AiShaderEvalParamVec(p_lookup_vector);
   ddx = AiShaderEvalParamVec(p_lookup_vector_ddx);
   ddy = AiShaderEvalParamVec(p_lookup_vector_ddy);
   
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
   
   sg->out.RGB = AiShaderEvalParamRGB(p_input);
   
   uvs.restore(sg);
}
