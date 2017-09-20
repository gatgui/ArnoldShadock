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

AI_SHADER_NODE_EXPORT_METHODS(RayStateVMtd);

enum RayStateVParams
{
   p_state = 0,
   p_ray,
   p_default
};

enum RayState
{
   RS_origin = 0,
   RS_dir,
   RS_dOdx,
   RS_dOdy,
   RS_dDdx,
   RS_dDdy
};

static const char* RayStateNames[] =
{
   "origin",
   "dir",
   "dOdx",
   "dOdy",
   "dDdx",
   "dDdy",
   NULL
};

node_parameters
{
   AiParameterEnum(SSTR::state, RS_origin, RayStateNames);
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterVec("default", 0.0f, 0.0f, 0.0f);
}

struct RayStateVData
{
   int state;
};

node_initialize
{
   AiNodeSetLocalData(node, new RayStateVData());
   AddMemUsage<RayStateVData>();
}

node_update
{
   RayStateVData *data = (RayStateVData*) AiNodeGetLocalData(node);
   data->state = AiNodeGetInt(node, SSTR::state);
}

node_finish
{
   RayStateVData *data = (RayStateVData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<RayStateVData>();
}

shader_evaluate
{
   AtRay *ray = 0;
   
   AiShaderEvalParamVec(p_ray);
   
   if (!AiStateGetMsgPtr(SSTR::agsb_ray, (void**)&ray) || !ray)
   {
      sg->out.VEC() = AiShaderEvalParamVec(p_default);
   }
   else
   {
      RayStateVData *data = (RayStateVData*) AiNodeGetLocalData(node);
      
      switch (data->state)
      {
      case RS_origin:
         sg->out.VEC() = ray->origin;
         break;
      case RS_dir:
         sg->out.VEC() = ray->dir;
         break;
      case RS_dOdx:
         sg->out.VEC() = ray->dOdx;
         break;
      case RS_dOdy:
         sg->out.VEC() = ray->dOdy;
         break;
      case RS_dDdx:
         sg->out.VEC() = ray->dDdx;
         break;
      case RS_dDdy:
         sg->out.VEC() =  ray->dDdy;
         break;
      default:
         sg->out.VEC() = AiShaderEvalParamVec(p_default);
         break;
      }
   }
}
