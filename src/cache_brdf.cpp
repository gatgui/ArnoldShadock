#include "cache.h"

AI_SHADER_NODE_EXPORT_METHODS(CacheBrdfMtd);

enum CacheBrdfParams
{
   p_input = 0
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
}

typedef Cache<BRDFData*> BrdfCache;

node_initialize
{
   AiNodeSetLocalData(node, AllocCache<BRDFData*>(0));
}

node_update
{
   BrdfCache *cache = (BrdfCache*) AiNodeGetLocalData(node);
   
   ReAllocCache(cache, GetRenderThreadsCount());
   EmptyCache(cache);
}

node_finish
{
   BrdfCache *cache = (BrdfCache*) AiNodeGetLocalData(node);
   
   FreeCache(cache);
}

shader_evaluate
{
   BrdfCache *cache = (BrdfCache*) AiNodeGetLocalData(node);
   
   BRDFData *brdf_data = 0;
   
   if (!GetCacheValue(sg, cache, brdf_data))
   {
      AiShaderEvalParamRGB(p_input);
      AiStateGetMsgPtr("agsb_brdf", (void**)&brdf_data);
      
      SetCacheValue(sg, cache, brdf_data);
   }
   else
   {
      AiStateSetMsgPtr("agsb_brdf", brdf_data);
   }
   
   sg->out.RGB = AI_RGB_BLACK;
}
