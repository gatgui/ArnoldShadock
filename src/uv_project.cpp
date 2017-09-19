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
   UV_lat_long,
   UV_mirrored_ball
};

static const char* UVModeNames[] =
{
   "planar",
   "triplanar",
   "perspective",
   "angular_map",
   "lat_long",
   "mirrored_ball",
   NULL
};

// ---

node_parameters
{
   AtMatrix id = AiM4Identity();
   
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterEnum(SSTR::lookup_vector, LV_N, LookupVectorNames);
   AiParameterEnum(SSTR::lookup_vector_space, LVS_world, LookupVectorSpaceNames);
   AiParameterEnum(SSTR::mode, UV_lat_long, UVModeNames);
   AiParameterVec(SSTR::custom_vector, 0.0f, 0.0f, 1.0f);
   AiParameterBool(SSTR::custom_is_point, false);
   AiParameterVec("custom_vector_ddx", 0.0f, 0.0f, 0.0f);
   AiParameterVec("custom_vector_ddy", 0.0f, 0.0f, 0.0f);
   AiParameterMtx("offset_matrix", id);
   AiParameterBool(SSTR::recompute_surface_uv_derivs, false);
}

struct UVProjectData
{
   LookupVector lookupVector;
   LookupVectorSpace space;
   bool useCustomVector;
   bool customIsPoint;
   UVMode mode;
   bool recomputeSurfaceUVDerivs;
};

node_initialize
{
   AiNodeSetLocalData(node, new UVProjectData());
   AddMemUsage<UVProjectData>();
}

node_update
{
   UVProjectData *data = (UVProjectData*) AiNodeGetLocalData(node);
   
   data->lookupVector = (LookupVector) AiNodeGetInt(node, SSTR::lookup_vector);
   data->space = (LookupVectorSpace) AiNodeGetInt(node, SSTR::lookup_vector_space);
   data->useCustomVector = AiNodeIsLinked(node, SSTR::custom_vector);
   data->customIsPoint = AiNodeGetBool(node, SSTR::custom_is_point);
   data->mode = (UVMode) AiNodeGetInt(node, SSTR::mode);
   data->recomputeSurfaceUVDerivs = AiNodeGetBool(node, SSTR::recompute_surface_uv_derivs);
}

node_finish
{
   UVProjectData *data = (UVProjectData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<UVProjectData>();
}

shader_evaluate
{
   AtVector dPdx, dPdy, v0, v1, v2;
   AtHPoint d, ddx, ddy, h;
   AtMatrix M, *Moff = 0;
   bool withDerivatives = false;
   
   UVData uvs(sg);
   
   UVProjectData *data = (UVProjectData*) AiNodeGetLocalData(node);
   
   if (data->recomputeSurfaceUVDerivs)
   {
      // Compute acutal dPdx and dPdv from dudx, dudy, dvdx and dvdy
      ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   }
   
   Moff = AiShaderEvalParamMtx(p_offset_matrix);
   
   if (data->useCustomVector)
   {
      v0 = AiShaderEvalParamVec(p_custom_vector);
      d = AtHPoint(v0, (data->customIsPoint ? 1.0f : 0.0f));
      
      v1 = AiShaderEvalParamVec(p_custom_vector_ddx);
      
      v2 = AiShaderEvalParamVec(p_custom_vector_ddy);
      
      if (!AiV3IsSmall(v1) && !AiV3IsSmall(v2))
      {
         withDerivatives = true;
         ddx = AtHPoint(v0 + v1, d.w);
         ddy = AtHPoint(v0 + v2, d.w);
      }

      M = *Moff;
   }
   else
   {
      withDerivatives = true;
      
      if (data->lookupVector == LV_P)
      {
         AiV4CreatePoint(d, sg->P);
         ddx = AtHPoint(sg->P.x + sg->dPdx.x, sg->P.y + sg->dPdx.y, sg->P.z + sg->dPdx.z, 1.0f);
         ddy = AtHPoint(sg->P.x + sg->dPdy.x, sg->P.y + sg->dPdy.y, sg->P.z + sg->dPdy.z, 1.0f);
      }
      else
      {
         AiV4CreateVector(d, sg->N);
         ddx = AtHPoint(sg->N.x + sg->dNdx.x, sg->N.y + sg->dNdx.y, sg->N.z + sg->dNdx.z, 0.0f);
         ddy = AtHPoint(sg->N.x + sg->dNdy.x, sg->N.y + sg->dNdy.y, sg->N.z + sg->dNdy.z, 0.0f);
         withDerivatives = false;
      }
      
      if (data->space == LVS_object)
      {
         M = AiM4Mult(sg->Minv, *Moff);
      }
      else
      {
         M = *Moff;
      }
   }
   
   h = AiM4HPointByMatrixMult(M, d);
   d = h;
   h = AiM4HPointByMatrixMult(M, ddx);
   ddx = h;
   h = AiM4HPointByMatrixMult(M, ddy);
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
      v0 = AtVector(d.x, d.y, d.z);
      if (withDerivatives)
      {
         v1 = AtVector(ddx.x, ddx.y, ddx.z);
         v2 = AtVector(ddy.x, ddy.y, ddy.z);
         v1 -= v0;
         v2 -= v0;
         AiMappingAngularMapDerivs(v0, v1, v2, sg->u, sg->v, sg->dudx, sg->dudy, sg->dvdx, sg->dvdy);
      }
      else
      {
         AiMappingAngularMap(v0, sg->u, sg->v);
      }
      break;
   case UV_lat_long:
      v0 = AtVector(d.x, d.y, d.z);
      if (withDerivatives)
      {
         v1 = AtVector(ddx.x, ddx.y, ddx.z);
         v2 = AtVector(ddy.x, ddy.y, ddy.z);
         v1 -= v0;
         v2 -= v0;
         AiMappingLatLongDerivs(v0, v1, v2, sg->u, sg->v, sg->dudx, sg->dudy, sg->dvdx, sg->dvdy);
      }
      else
      {
         AiMappingLatLong(v0, sg->u, sg->v);
      }
      break;
   case UV_mirrored_ball:
      v0 = AtVector(d.x, d.y, d.z);
      if (withDerivatives)
      {
         v1 = AtVector(ddx.x, ddx.y, ddx.z);
         v2 = AtVector(ddy.x, ddy.y, ddy.z);
         v1 -= v0;
         v2 -= v0;
         AiMappingMirroredBallDerivs(v0, v1, v2, sg->u, sg->v, sg->dudx, sg->dudy, sg->dvdx, sg->dvdy);
      }
      else
      {
         AiMappingMirroredBall(v0, sg->u, sg->v);
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
   else if (data->recomputeSurfaceUVDerivs)
   {
      // Re-compute dPdu and dPdv from dPdx, dPdy, dudx, dudy, dvdx, dvdy (invariant)
      ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
   }
   
   sg->out.RGBA() = AiShaderEvalParamRGBA(p_input);
   
   uvs.restore(sg);
}
