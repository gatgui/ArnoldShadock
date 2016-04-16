#include "cache.h"

AI_SHADER_NODE_EXPORT_METHODS(CacheFMtd);

enum CacheFParams
{
   p_input = 0
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
}

typedef Cache<float> FloatCache;

node_initialize
{
   AiNodeSetLocalData(node, AllocCache<float>(0));
}

node_update
{
   FloatCache *cache = (FloatCache*) AiNodeGetLocalData(node);
   ReAllocCache(cache, GetRenderThreadsCount());
   EmptyCache(cache);
}

node_finish
{
   FloatCache *cache = (FloatCache*) AiNodeGetLocalData(node);
   FreeCache(cache);
}

shader_evaluate
{
   FloatCache *cache = (FloatCache*) AiNodeGetLocalData(node);
   
   if (!GetCacheValue(sg, cache, sg->out.FLT))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_input);
      SetCacheValue(sg, cache, sg->out.FLT);
   }
}
