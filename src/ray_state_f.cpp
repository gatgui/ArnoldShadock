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

AI_SHADER_NODE_EXPORT_METHODS(RayStateFMtd);

enum RayStateFParams
{
   p_state = 0,
   p_ray,
   p_default
};

enum RayState
{
   RS_sx = 0,
   RS_sy,
   RS_px,
   RS_py,
   RS_mindist,
   RS_maxdist,
   RS_weight,
   RS_time
};

static const char* RayStateNames[] =
{
   "sx",
   "sy",
   "px",
   "py",
   "mindist",
   "maxdist",
   "weight",
   "time",
   NULL
};

node_parameters
{
   AiParameterEnum(SSTR::state, RS_sx, RayStateNames);
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("default", 0.0f);
}

struct RayStateFData
{
   int state;
};

node_initialize
{
   AiNodeSetLocalData(node, new RayStateFData());
   AddMemUsage<RayStateFData>();
}

node_update
{
   RayStateFData *data = (RayStateFData*) AiNodeGetLocalData(node);
   data->state = AiNodeGetInt(node, SSTR::state);
}

node_finish
{
   RayStateFData *data = (RayStateFData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<RayStateFData>();
}

shader_evaluate
{
   AtRay *ray = 0;
   
   AiShaderEvalParamVec(p_ray);
   
   if (!AiStateGetMsgPtr(SSTR::agsb_ray, (void**)&ray) || !ray)
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default);
   }
   else
   {
      RayStateFData *data = (RayStateFData*) AiNodeGetLocalData(node);
      
      switch (data->state)
      {
      case RS_sx:
         sg->out.FLT = ray->sx;
         break;
      case RS_sy:
         sg->out.FLT = ray->sy;
         break;
      case RS_px:
         sg->out.FLT = ray->px;
         break;
      case RS_py:
         sg->out.FLT = ray->py;
         break;
      case RS_mindist:
         sg->out.FLT = ray->mindist;
         break;
      case RS_maxdist:
         sg->out.FLT =  ray->maxdist;
         break;
      case RS_weight:
         sg->out.FLT = ray->weight;
         break;
      case RS_time:
         sg->out.FLT = ray->time;
         break;
      default:
         sg->out.FLT = AiShaderEvalParamFlt(p_default);
         break;
      }
   }
}
