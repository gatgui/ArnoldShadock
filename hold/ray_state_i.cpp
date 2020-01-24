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

AI_SHADER_NODE_EXPORT_METHODS(RayStateIMtd);

enum RayStateIParams
{
   p_state = 0,
   p_ray,
   p_default
};

enum RayState
{
   RS_type = 0,
   RS_bounces,
   RS_bounces_diffuse,
   RS_bounces_specular,
   RS_bounces_reflect,
   RS_bounces_transmit,
   RS_bounces_volume,
   RS_x,
   RS_y
};

static const char* RayStateNames[] =
{
   "type",
   "bounces",
   "bounces_diffuse",
   "bounces_specular",
   "bounces_reflect",
   "bounces_transmit",
   "bounces_volume",
   "x",
   "y",
   NULL
};

node_parameters
{
   AiParameterEnum(SSTR::state, RS_type, RayStateNames);
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterInt("default", 0);
}

struct RayStateIData
{
   int state;
};

node_initialize
{
   AiNodeSetLocalData(node, new RayStateIData());
   AddMemUsage<RayStateIData>();
}

node_update
{
   RayStateIData *data = (RayStateIData*) AiNodeGetLocalData(node);
   data->state = AiNodeGetInt(node, SSTR::state);
}

node_finish
{
   RayStateIData *data = (RayStateIData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<RayStateIData>();
}

shader_evaluate
{
   AtRay *ray = 0;
   
   AiShaderEvalParamVec(p_ray);
   
   if (!AiStateGetMsgPtr(SSTR::agsb_ray, (void**)&ray) || !ray)
   {
      sg->out.INT() = AiShaderEvalParamInt(p_default);
   }
   else
   {
      RayStateIData *data = (RayStateIData*) AiNodeGetLocalData(node);

      switch (data->state)
      {
      case RS_type:
         sg->out.INT() = ray->type;
         break;
      case RS_bounces:
         sg->out.INT() = ray->bounces;
         break;
      case RS_bounces_diffuse:
         sg->out.INT() = ray->bounces_diffuse;
         break;
      case RS_bounces_specular:
         sg->out.INT() = ray->bounces_specular;
         break;
      case RS_bounces_reflect:
         sg->out.INT() = ray->bounces_reflect;
         break;
      case RS_bounces_transmit:
         sg->out.INT() =  ray->bounces_transmit;
         break;
      case RS_bounces_volume:
         sg->out.INT() =  ray->bounces_volume;
         break;
      case RS_x:
         sg->out.INT() = ray->x;
         break;
      case RS_y:
         sg->out.INT() = ray->y;
         break;
      default:
         sg->out.INT() = AiShaderEvalParamInt(p_default);
         break;
      }
   }
}
