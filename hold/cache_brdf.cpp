/*
MIT License
Copyright (c) 2014~ Gaetan Guidet
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
      AiStateGetMsgPtr(SSTR::agsb_brdf, (void**)&brdf_data);
      
      SetCacheValue(sg, cache, brdf_data);
   }
   else
   {
      AiStateSetMsgPtr(SSTR::agsb_brdf, brdf_data);
   }
   
   sg->out.RGB = AI_RGB_BLACK;
}
