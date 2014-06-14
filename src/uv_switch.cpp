#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVSwitchMtd);

enum UVSwitchParams
{
   p_input = 0,
   p_mode,
   p_uv_set_name,
   p_lookup_vector,
   p_lookup_vector_ddx,
   p_lookup_vector_ddy
};

enum UVMode
{
   UV_angular_map = 0,
   UV_cubic_map ,
   UV_lat_long,
   UV_mirrored_ball,
   UV_user_defined
};

static const char* UVModeNames[] =
{
   "angular_map",
   "cubic_map",
   "lat_long",
   "mirrored_ball",
   "user_defined",
   NULL
};

// ---

struct UVData
{
   float u, v;
   float dudx, dudy;
   float dvdx, dvdy;
   AtVector dPdu, dPdv;
   
   void store(AtShaderGlobals *sg)
   {
      u = sg->u;
      v = sg->v;
      dudx = sg->dudx;
      dudy = sg->dudy;
      dvdx = sg->dvdx;
      dvdy = sg->dvdy;
      dPdu = sg->dPdu;
      dPdv = sg->dPdv;
   }
   
   void restore(AtShaderGlobals *sg)
   {
      sg->u = u;
      sg->v = v;
      sg->dudx = dudx;
      sg->dudy = dudy;
      sg->dvdx = dvdx;
      sg->dvdy = dvdy;
      sg->dPdu = dPdu;
      sg->dPdv = dPdv;
   }
};

void ComputeSurfaceScreenDerivatives(AtShaderGlobals *sg, AtVector &dPdx, AtVector &dPdy)
{
   dPdx = sg->dudx * sg->dPdu + sg->dvdx * sg->dPdv;
   dPdy = sg->dudy * sg->dPdu + sg->dvdy * sg->dPdv;
}

void ComputeSurfaceUVDerivatives(AtShaderGlobals *sg, const AtVector &dPdx, const AtVector &dPdy)
{
   // (1) dPdx = dudx * dPdu + dvdx * dPdv
   // (2) dPdy = dudy * dPdu + dvdy * dPdv

   // Note 1: sg's dPdu and dPdv are left unchanged when the system cannot be solved
   // Note 2: Could scale all equations to gain precision
   
   float rv[6];
   LinearSystem<6> lsys;

   lsys[0][0] = sg->dudx; lsys[0][3] = sg->dvdx; lsys[0][6] = dPdx.x;
   lsys[1][1] = sg->dudx; lsys[1][4] = sg->dvdx; lsys[1][6] = dPdx.y;
   lsys[2][2] = sg->dudx; lsys[2][5] = sg->dvdx; lsys[2][6] = dPdx.z;
   lsys[3][0] = sg->dudy; lsys[3][3] = sg->dvdy; lsys[3][6] = dPdy.x;
   lsys[4][1] = sg->dudy; lsys[4][4] = sg->dvdy; lsys[4][6] = dPdy.y;
   lsys[5][2] = sg->dudy; lsys[5][5] = sg->dvdy; lsys[5][6] = dPdy.z;

   if (lsys.solve(rv))
   {
      sg->dPdu.x = rv[0];
      sg->dPdu.y = rv[1];
      sg->dPdu.z = rv[2];
      sg->dPdv.x = rv[3];
      sg->dPdv.y = rv[4];
      sg->dPdv.z = rv[5];
   }
   else
   {
      AiMsgWarning("Could no re-compute dPdu and dPdv");
   }
}

// ---

struct NodeData
{
   UVMode mode;
   bool linked_uv_set_name;
   const char *uv_set_name;
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("mode", UV_user_defined, UVModeNames);
   AiParameterStr("uv_set_name", "");
   AiParameterVec("lookup_vector", 0.0f, 0.0f, 1.0f);
   AiParameterVec("lookup_vector_ddx", 0.0f, 0.0f, 0.0f);
   AiParameterVec("lookup_vector_ddy", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "mode", "linkable", false);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->mode = (UVMode) AiNodeGetInt(node, "mode");
   data->linked_uv_set_name = false;
   data->uv_set_name = 0;
   
   if (data->mode == UV_user_defined)
   {
      data->linked_uv_set_name = AiNodeIsLinked(node, "uv_set_name");
      if (!data->linked_uv_set_name)
      {
         data->uv_set_name = AiNodeGetStr(node, "uv_set_name");
      }
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   UVData uvs;
   AtVector dPdx, dPdy;
   
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   // Backup curent shading globals state
   uvs.store(sg);
   
   // Compute acutal dPdx and dPdv from dudx, dudy, dvdx and dvdy
   ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   
   switch (data->mode)
   {
   case UV_angular_map:
      {
         AtVector d = AiShaderEvalParamVec(p_lookup_vector);
         AtVector ddx = AiShaderEvalParamVec(p_lookup_vector_ddx);
         AtVector ddy = AiShaderEvalParamVec(p_lookup_vector_ddy);
         
         if (AiV3IsZero(ddx) && AiV3IsZero(ddy))
         {
            AiMappingAngularMap(&d, &(sg->u), &(sg->v));
            
            sg->dudx = 0.0f;
            sg->dudy = 0.0f;
            sg->dvdx = 0.0f;
            sg->dvdy = 0.0f;
            
            // dPdu and dPdv are undefined
         }
         else
         {
            AiMappingAngularMapDerivs(&d, &ddx, &ddy, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
            
            // Re-compute dPdu and dPdv from dPdx and dPdy (invariant)
            ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
         }
      }
      break;
   case UV_cubic_map:
      {
         AtVector d = AiShaderEvalParamVec(p_lookup_vector);
         AtVector ddx = AiShaderEvalParamVec(p_lookup_vector_ddx);
         AtVector ddy = AiShaderEvalParamVec(p_lookup_vector_ddy);
         
         if (AiV3IsZero(ddx) && AiV3IsZero(ddy))
         {
            AiMappingCubicMap(&d, &(sg->u), &(sg->v));
            
            sg->dudx = 0.0f;
            sg->dudy = 0.0f;
            sg->dvdx = 0.0f;
            sg->dvdy = 0.0f;
            
            // dPdu and dPdv are undefined
         }
         else
         {
            AiMappingCubicMapDerivs(&d, &ddx, &ddy, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
            
            // Re-compute dPdu and dPdv from dPdx and dPdy (invariant)
            ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
         }
      }
      break;
   case UV_lat_long:
      {
         AtVector d = AiShaderEvalParamVec(p_lookup_vector);
         AtVector ddx = AiShaderEvalParamVec(p_lookup_vector_ddx);
         AtVector ddy = AiShaderEvalParamVec(p_lookup_vector_ddy);
         
         if (AiV3IsZero(ddx) && AiV3IsZero(ddy))
         {
            AiMappingLatLong(&d, &(sg->u), &(sg->v));
            
            sg->dudx = 0.0f;
            sg->dudy = 0.0f;
            sg->dvdx = 0.0f;
            sg->dvdy = 0.0f;
            
            // dPdu and dPdv are undefined
         }
         else
         {
            AiMappingLatLongDerivs(&d, &ddx, &ddy, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
            
            // Re-compute dPdu and dPdv from dPdx and dPdy (invariant)
            ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
         }
      }
      break;
   case UV_mirrored_ball:
      {
         AtVector d = AiShaderEvalParamVec(p_lookup_vector);
         AtVector ddx = AiShaderEvalParamVec(p_lookup_vector_ddx);
         AtVector ddy = AiShaderEvalParamVec(p_lookup_vector_ddy);
         
         if (AiV3IsZero(ddx) && AiV3IsZero(ddy))
         {
            AiMappingMirroredBall(&d, &(sg->u), &(sg->v));
            
            sg->dudx = 0.0f;
            sg->dudy = 0.0f;
            sg->dvdx = 0.0f;
            sg->dvdy = 0.0f;
            
            // dPdu and dPdv are undefined
         }
         else
         {
            AiMappingMirroredBallDerivs(&d, &ddx, &ddy, &(sg->u), &(sg->v), &(sg->dudx), &(sg->dudy), &(sg->dvdx), &(sg->dvdy));
            
            // Re-compute dPdu and dPdv from dPdx and dPdy (invariant)
            ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
         }
      }
      break;
   case UV_user_defined:
      {
         AtPoint2 altuv, altuvDx, altuvDy;
         
         const char *uv_set_name = (data->linked_uv_set_name ? AiShaderEvalParamStr(p_uv_set_name) : data->uv_set_name);
         
         if (AiUDataGetPnt2(uv_set_name, &altuv))
         {
            sg->u = altuv.x;
            sg->v = altuv.y;
            
            if (AiUDataGetDxyDerivativesPnt2(uv_set_name, &altuvDx, &altuvDy)) 
            { 
               sg->dudx = altuvDx.x; 
               sg->dvdx = altuvDx.y; 
               sg->dudy = altuvDy.x; 
               sg->dvdy = altuvDy.y;
               
               // Re-compute dPdu and dPdv from dPdx and dPdy (invariant)
               ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
            }
            else
            {
               sg->dudx = 0.0f;
               sg->dudy = 0.0f;
               sg->dvdx = 0.0f;
               sg->dvdy = 0.0f;
               
               // dPdu and dPdv are undefined
            }
         }
      }
      break;
   default:
      break;
   }
   
   sg->out.RGB = AiShaderEvalParamRGB(p_input);
   
   uvs.restore(sg);
}
