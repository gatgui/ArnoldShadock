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
   AiParameterEnum(SSTR::state, RS_type, RayStateNames);
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
   AiParameterInt("default", 0);
   
   AiMetaDataSetBool(mds, SSTR::state, SSTR::linkable, false);
}

struct NodeData
{
   int state;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   data->state = AiNodeGetInt(node, SSTR::state);
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   AtRay *ray = 0;
   
   AiShaderEvalParamVec(p_ray);
   
   if (!AiStateGetMsgPtr(SSTR::agsb_ray, (void**)&ray) || !ray)
   {
      sg->out.INT = AiShaderEvalParamInt(p_default);
   }
   else
   {
      NodeData *data = (NodeData*) AiNodeGetLocalData(node);
      
      switch (data->state)
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
