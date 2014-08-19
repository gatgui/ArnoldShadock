#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SamplerMtd);

enum SamplerParams
{
   p_samples = 0,
   p_seeded,
   p_seed
};

node_parameters
{
   AiParameterInt("samples", 1);
   AiParameterBool("seeded", false);
   AiParameterInt("seed", 0);
   
   AiMetaDataSetBool(mds, "samples", "linkable", false);
   AiMetaDataSetBool(mds, "seeded", "linkable", false);
   AiMetaDataSetBool(mds, "seed", "linkable", false);
}

node_initialize
{
   AiNodeSetLocalData(node, 0);
}

node_update
{
   AtSampler *sampler = (AtSampler*) AiNodeGetLocalData(node);
   
   if (sampler)
   {
      AiSamplerDestroy(sampler);
   }
   
   if (AiNodeGetBool(node, "seeded"))
   {
      sampler = AiSamplerSeeded(AiNodeGetInt(node, "seed"), AiNodeGetInt(node, "samples"), 2);
   }
   else
   {
      sampler = AiSampler(AiNodeGetInt(node, "samples"), 2);
   }
   
   AiNodeSetLocalData(node, sampler);
}

node_finish
{
   AtSampler *sampler = (AtSampler*) AiNodeGetLocalData(node);
   
   if (sampler)
   {
      AiSamplerDestroy(sampler);
   }
}

shader_evaluate
{
   AiStateSetMsgPtr("agsb_sampler", AiNodeGetLocalData(node));
   sg->out.RGB = AI_RGB_BLACK;
}
