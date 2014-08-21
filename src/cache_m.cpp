#include "cache.h"

AI_SHADER_NODE_EXPORT_METHODS(CacheMMtd);

enum CacheMParams
{
   p_input = 0
};

node_parameters
{
   AiParameterMtx("input", AI_M4_IDENTITY);
}

typedef Cache<AtMatrix*> MatrixCache;

node_initialize
{
   AiNodeSetLocalData(node, AllocCache<AtMatrix*>(0));
}

node_update
{
   MatrixCache *cache = (MatrixCache*) AiNodeGetLocalData(node);
   
   ReAllocCache(cache, GetRenderThreadsCount());
   EmptyCache(cache);
}

node_finish
{
   MatrixCache *cache = (MatrixCache*) AiNodeGetLocalData(node);
   
   FreeCache(cache);
}

shader_evaluate
{
   MatrixCache *cache = (MatrixCache*) AiNodeGetLocalData(node);
   
   if (!GetCacheValue(sg, cache, sg->out.pMTX))
   {
      sg->out.pMTX = AiShaderEvalParamMtx(p_input);
      SetCacheValue(sg, cache, sg->out.pMTX);
   }
}
