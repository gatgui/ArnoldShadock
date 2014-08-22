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
   RS_level,
   RS_diff_bounces,
   RS_gloss_bounces,
   RS_refl_bounces,
   RS_refr_bounces,
   RS_x,
   RS_y
};

static const char* RayStateNames[] =
{
   "type",
   "level",
   "diff_bounces",
   "gloss_bounces",
   "refl_bounces",
   "refr_bounces",
   "x",
   "y",
   NULL
};

node_parameters
{
   AiParameterEnum("state", RS_type, RayStateNames);
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterInt("default", 0);
   
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
      sg->out.INT = AiShaderEvalParamInt(p_default);
   }
   else
   {
      void *data = AiNodeGetLocalData(node);
      RayState state = (RayState) size_t(data);
      
      switch (state)
      {
      case RS_type:
         sg->out.INT = ray->type;
         break;
      case RS_level:
         sg->out.INT = ray->level;
         break;
      case RS_diff_bounces:
         sg->out.INT = ray->diff_bounces;
         break;
      case RS_gloss_bounces:
         sg->out.INT = ray->gloss_bounces;
         break;
      case RS_refl_bounces:
         sg->out.INT = ray->refl_bounces;
         break;
      case RS_refr_bounces:
         sg->out.INT =  ray->refr_bounces;
         break;
      case RS_x:
         sg->out.INT = ray->x;
         break;
      case RS_y:
         sg->out.INT = ray->y;
         break;
      default:
         sg->out.INT = AiShaderEvalParamInt(p_default);
         break;
      }
   }
}
