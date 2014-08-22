#include "cache.h"

AI_SHADER_NODE_EXPORT_METHODS(CacheVMtd);

enum CacheVParams
{
   p_input = 0
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
}

typedef Cache<AtVector> VectorCache;

node_initialize
{
   AiNodeSetLocalData(node, AllocCache<AtVector>(0));
}

node_update
{
   VectorCache *cache = (VectorCache*) AiNodeGetLocalData(node);
   
   ReAllocCache(cache, GetRenderThreadsCount());
   EmptyCache(cache);
}

node_finish
{
   VectorCache *cache = (VectorCache*) AiNodeGetLocalData(node);
   
   FreeCache(cache);
}

shader_evaluate
{
   VectorCache *cache = (VectorCache*) AiNodeGetLocalData(node);
   
   if (!GetCacheValue(sg, cache, sg->out.VEC))
   {
      sg->out.VEC = AiShaderEvalParamVec(p_input);
      SetCacheValue(sg, cache, sg->out.VEC);
   }
}
