#include "cache.h"

AI_SHADER_NODE_EXPORT_METHODS(CacheRayMtd);

enum CacheRayParams
{
   p_input = 0
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
}

typedef Cache<AtRay*> RayCache;

node_initialize
{
   AiNodeSetLocalData(node, AllocCache<AtRay*>(0));
}

node_update
{
   RayCache *cache = (RayCache*) AiNodeGetLocalData(node);
   
   ReAllocCache(cache, GetRenderThreadsCount());
   EmptyCache(cache);
}

node_finish
{
   RayCache *cache = (RayCache*) AiNodeGetLocalData(node);
   
   FreeCache(cache);
}

shader_evaluate
{
   RayCache *cache = (RayCache*) AiNodeGetLocalData(node);
   
   AtRay *ray = 0;
   
   if (!GetCacheValue(sg, cache, ray))
   {
      AiShaderEvalParamVec(p_input);
      AiStateGetMsgPtr("agsb_ray", (void**)&ray);
      
      SetCacheValue(sg, cache, ray);
   }
   else
   {
      AiStateSetMsgPtr("agsb_ray", ray);
   }
   
   sg->out.RGB = AI_RGB_BLACK;
}
