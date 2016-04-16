#include "cache.h"

AI_SHADER_NODE_EXPORT_METHODS(CacheC3Mtd);

enum CacheC3Params
{
   p_input = 0
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
}

typedef Cache<AtColor> ColorCache;

node_initialize
{
   AiNodeSetLocalData(node, AllocCache<AtColor>(0));
}

node_update
{
   ColorCache *cache = (ColorCache*) AiNodeGetLocalData(node);
   ReAllocCache(cache, GetRenderThreadsCount());
   EmptyCache(cache);
}

node_finish
{
   ColorCache *cache = (ColorCache*) AiNodeGetLocalData(node);
   FreeCache(cache);
}

shader_evaluate
{
   ColorCache *cache = (ColorCache*) AiNodeGetLocalData(node);
   
   if (!GetCacheValue(sg, cache, sg->out.RGB))
   {
      sg->out.RGB = AiShaderEvalParamRGB(p_input);
      SetCacheValue(sg, cache, sg->out.RGB);
   }
}
