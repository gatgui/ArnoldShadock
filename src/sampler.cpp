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
   AiParameterInt(SSTR::samples, 1);
   AiParameterBool(SSTR::seeded, false);
   AiParameterInt(SSTR::seed, 0);
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
   
   if (AiNodeGetBool(node, SSTR::seeded))
   {
      sampler = AiSamplerSeeded(AiNodeGetInt(node, SSTR::seed), AiNodeGetInt(node, SSTR::samples), 2);
   }
   else
   {
      sampler = AiSampler(AiNodeGetInt(node, SSTR::samples), 2);
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
   AiStateSetMsgPtr(SSTR::agsb_sampler, AiNodeGetLocalData(node));
   sg->out.RGB = AI_RGB_BLACK;
}
