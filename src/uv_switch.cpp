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

struct UVSwitchData
{
   AtString uvSetName;
   bool recomputeSurfaceUVDerivs;
};

node_initialize
{
   AiNodeSetLocalData(node, new UVSwitchData());
   AddMemUsage<UVSwitchData>();
}

node_update
{
   UVSwitchData *data = (UVSwitchData*) AiNodeGetLocalData(node);
   
   data->uvSetName = AiNodeGetStr(node, SSTR::uv_set_name);
   data->recomputeSurfaceUVDerivs = AiNodeGetBool(node, SSTR::recompute_surface_uv_derivs);
}

node_finish
{
   UVSwitchData *data = (UVSwitchData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<UVSwitchData>();
}

shader_evaluate
{
   AtVector dPdx, dPdy;
   AtVector2 altuv, altuvDx, altuvDy;
   
   UVData uvs(sg);
   
   UVSwitchData *data = (UVSwitchData*) AiNodeGetLocalData(node);
   
   if (data->recomputeSurfaceUVDerivs)
   {
      // Compute acutal dPdx and dPdv from dudx, dudy, dvdx and dvdy
      ComputeSurfaceScreenDerivatives(sg, dPdx, dPdy);
   }
   
   if (AiUDataGetVec2(data->uvSetName, altuv))
   {
      sg->u = altuv.x;
      sg->v = altuv.y;
      
      if (AiUDataGetDxyDerivativesVec2(data->uvSetName, altuvDx, altuvDy)) 
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
   
   sg->out.RGBA() = AiShaderEvalParamRGBA(p_input);
   
   uvs.restore(sg);
}
