#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RayStateFloatMtd);

enum RayStateFloatParams
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
   AiParameterEnum("state", RS_sx, RayStateNames);
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("default", 0.0f);
   
   AiMetaDataSetBool(mds, "state", "linkable", false);
}

node_initialize
{
}

node_update
{
   AiNodeSetLocalData(node, (void*) AiNodeGetInt(node, "state"));
}

node_finish
{
}

shader_evaluate
{
   AtRay *ray = 0;
   
   AiShaderEvalParamVec(p_ray);
   
   if (!AiStateGetMsgPtr("agsb_ray", (void**)&ray) || !ray)
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default);
   }
   else
   {
      void *data = AiNodeGetLocalData(node);
      RayState state = (RayState) *((int*)&data);
      
      switch (state)
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
