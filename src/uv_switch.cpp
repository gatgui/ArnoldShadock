#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVSwitchMtd);

enum UVSwitchParams
{
   p_input = 0,
   p_uv_set_name,
   p_recompute_surface_uv_derivs
};

struct UVSwitchData
{
   bool linked_uv_set_name;
   const char *uv_set_name;
   bool recompute_surface_uv_derivs;
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterStr("uv_set_name", "");
   AiParameterBool("recompute_surface_uv_derivs", false);
   
   AiMetaDataSetBool(mds, "recompute_surface_uv_derivs", "linkable", false);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(UVSwitchData)));
}

node_update
{
   UVSwitchData *data = (UVSwitchData*) AiNodeGetLocalData(node);
   
   data->uv_set_name = 0;
   data->linked_uv_set_name = AiNodeIsLinked(node, "uv_set_name");
   data->recompute_surface_uv_derivs = AiNodeGetBool(node, "recompute_surface_uv_derivs");
   
   if (!data->linked_uv_set_name)
   {
      data->uv_set_name = AiNodeGetStr(node, "uv_set_name");
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   AtVector dPdx, dPdy;
   AtPoint2 altuv, altuvDx, altuvDy;
   
   UVData uvs(sg);
   
   UVSwitchData *data = (UVSwitchData*) AiNodeGetLocalData(node);
   
   if (data->recompute_surface_uv_derivs)
   {
      // Compute acutal dPdx and dPdv from dudx, dudy, dvdx and dvdy
      ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   }
   
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
         
         if (data->recompute_surface_uv_derivs)
         {
            // Re-compute dPdu and dPdv from dPdx and dPdy (invariant)
            ComputeSurfaceUVDerivatives(sg, dPdx, dPdy);
         }
      }
      else
      {
         sg->dudx = 0.0f;
         sg->dudy = 0.0f;
         sg->dvdx = 0.0f;
         sg->dvdy = 0.0f;
         
         // dPdu and dPdv are undefined, leave unchanged
      }
   }
   
   sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   
   uvs.restore(sg);
}
