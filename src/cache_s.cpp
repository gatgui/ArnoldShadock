#include "cache.h"

AI_SHADER_NODE_EXPORT_METHODS(CacheSMtd);

enum CacheSParams
{
   p_input = 0
};

node_parameters
{
   AiParameterStr("input", "");
}

typedef Cache<const char*> StringCache;

node_initialize
{
   AiNodeSetLocalData(node, AllocCache<const char*>(0));
}

node_update
{
   StringCache *cache = (StringCache*) AiNodeGetLocalData(node);
   ReAllocCache(cache, GetRenderThreadsCount());
   EmptyCache(cache);
}

node_finish
{
   StringCache *cache = (StringCache*) AiNodeGetLocalData(node);
   FreeCache(cache);
}

shader_evaluate
{
   StringCache *cache = (StringCache*) AiNodeGetLocalData(node);
   
   if (!GetCacheValue(sg, cache, sg->out.STR))
   {
      sg->out.STR = AiShaderEvalParamStr(p_input);
      SetCacheValue(sg, cache, sg->out.STR);
   }
}
