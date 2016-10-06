#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(UVSwitchMtd);

enum UVSwitchParams
{
   p_input = 0,
   p_uv_set_name,
   p_recompute_surface_uv_derivs
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterStr(SSTR::uv_set_name, "");
   AiParameterBool(SSTR::recompute_surface_uv_derivs, false);
}

struct NodeData
{
   AtString uvSetName;
   bool recomputeSurfaceUVDerivs;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->uvSetName = AiNodeGetStr(node, SSTR::uv_set_name);
   data->recomputeSurfaceUVDerivs = AiNodeGetBool(node, SSTR::recompute_surface_uv_derivs);
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   AtVector dPdx, dPdy;
   AtPoint2 altuv, altuvDx, altuvDy;
   
   UVData uvs(sg);
   
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   if (data->recomputeSurfaceUVDerivs)
   {
      // Compute acutal dPdx and dPdv from dudx, dudy, dvdx and dvdy
      ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   }
   
   if (AiUDataGetPnt2(data->uvSetName, &altuv))
   {
      sg->u = altuv.x;
      sg->v = altuv.y;
      
      if (AiUDataGetDxyDerivativesPnt2(data->uvSetName, &altuvDx, &altuvDy)) 
      { 
         sg->dudx = altuvDx.x; 
         sg->dvdx = altuvDx.y; 
         sg->dudy = altuvDy.x; 
         sg->dvdy = altuvDy.y;
         
         if (data->recomputeSurfaceUVDerivs)
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
