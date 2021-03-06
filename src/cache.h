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

#ifndef __shadock_cache_h__
#define __shadock_cache_h__

#include "common.h"

template <typename T>
struct CacheEntry
{
   bool valid;
   T value;
   
   AtNode *Op;
   AtPoint P;
   AtVector N;
   float u;
   float v;
   AtPoint Ro;
};

template <typename T>
struct Cache
{
   typedef CacheEntry<T> EntryType;
   
   EntryType **entries;
   int nentries;
};

template <typename T>
Cache<T>* AllocCache(int nentries)
{
   Cache<T> *cache = (Cache<T>*) AiMalloc(sizeof(Cache<T>));
   
   cache->nentries = nentries;
   
   if (nentries > 0)
   {
      cache->entries = (CacheEntry<T>**) AiMalloc(nentries * sizeof(CacheEntry<T>*));
      
      for (int i=0; i<nentries; ++i)
      {
         cache->entries[i] = (CacheEntry<T>*) AiMalloc(sizeof(CacheEntry<T>));
      }
   }
   else
   {
      cache->entries = 0;
   }
   
   return cache;
}

template <typename T>
Cache<T>* ReAllocCache(Cache<T> *cache, int nentries)
{
   if (!cache)
   {
      return AllocCache<T>(nentries);
   }
   
   if (cache->nentries < nentries)
   {
      CacheEntry<T> **newentries = (CacheEntry<T>**) AiMalloc(nentries * sizeof(CacheEntry<T>*));
      
      for (int i=0; i<cache->nentries; ++i)
      {
         newentries[i] = cache->entries[i];
      }
      
      for (int i=cache->nentries; i<nentries; ++i)
      {
         newentries[i] = (CacheEntry<T>*) AiMalloc(sizeof(CacheEntry<T>));
      }
      
      if (cache->entries)
      {
         AiFree(cache->entries);
      }
      
      cache->nentries = nentries;
      cache->entries = newentries;
   }
   
   return cache;
}

template <typename T>
void EmptyCache(Cache<T> *cache)
{
   if (cache)
   {
      for (int i=0; i<cache->nentries; ++i)
      {
         cache->entries[i]->valid = false;
         cache->entries[i]->Op = 0;
      }
   }
}

template <typename T>
void FreeCache(Cache<T> *cache)
{
   if (cache)
   {
      for (int i=0; i<cache->nentries; ++i)
      {
         AiFree(cache->entries[i]);
      }
      
      AiFree(cache->entries);
      
      AiFree(cache);
   }
}

template <typename T>
bool SetCacheValue(AtShaderGlobals *sg, Cache<T> *cache, const T &value)
{
   if (!cache)
   {
      return false;
   }
   
   if (sg->tid >= cache->nentries)
   {
      return false;
   }
   
   CacheEntry<T> *entry = cache->entries[sg->tid];
   
   entry->valid = true;
   entry->Op = sg->Op;
   entry->P = sg->P;
   entry->Ro = sg->Ro;
   entry->N = sg->N;
   entry->u = sg->u;
   entry->v = sg->v;
   entry->value = value;
   
   return true;
}

template <typename T>
bool GetCacheValue(AtShaderGlobals *sg, Cache<T> *cache, T &value)
{
   if (!cache)
   {
      return false;
   }
   
   if (sg->tid >= cache->nentries)
   {
      return false;
   }
   
   CacheEntry<T> *entry = cache->entries[sg->tid];
   
   if (!entry->valid ||
       entry->Op != sg->Op ||
       entry->P != sg->P ||
       entry->Ro != sg->Ro ||
       entry->N != sg->N ||
       entry->u != sg->u ||
       entry->v != sg->v)
   {
      return false;
   }
   
   value = entry->value;
   
   return true;
}


#endif
