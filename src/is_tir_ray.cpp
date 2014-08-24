#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(IsTirRayMtd);

enum IsTirRayParams
{
   p_ray = 0
};

node_parameters
{
   AiParameterVec("ray", 0.0f, 0.0f, 0.0f);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   AtRay *ray = 0;
   
   if (AiStateGetMsgPtr("agsb_ray", (void**)&ray) && ray != 0)
   {
      // Only check agsb_tir if we have a valid ray
      bool tir = false;
      sg->out.BOOL = (AiStateGetMsgBool("agsb_tir", &tir) && tir);
   }
   else
   {
      sg->out.BOOL = false;
   }
}
