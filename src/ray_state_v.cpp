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
      sg->out.VEC = AiShaderEvalParamVec(p_default);
   }
   else
   {
      NodeData *data = (NodeData*) AiNodeGetLocalData(node);
      
      switch (data->state)
      {
      case RS_origin:
         sg->out.VEC = ray->origin;
         break;
      case RS_dir:
         sg->out.VEC = ray->dir;
         break;
      case RS_dOdx:
         sg->out.VEC = ray->dOdx;
         break;
      case RS_dOdy:
         sg->out.VEC = ray->dOdy;
         break;
      case RS_dDdx:
         sg->out.VEC = ray->dDdx;
         break;
      case RS_dDdy:
         sg->out.VEC =  ray->dDdy;
         break;
      default:
         sg->out.VEC = AiShaderEvalParamVec(p_default);
         break;
      }
   }
}
