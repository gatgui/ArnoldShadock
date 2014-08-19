#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SamplesLoopVectorMtd);

enum SamplesLoopVectorParams
{
   p_sampler = 0,
   p_input,
   p_combine
};

node_parameters
{
   AiParameterRGB("sampler", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("combine", CM_Add, CombineModeNames);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   AtSampler *sampler = 0;
   
   AiShaderEvalParamRGB(p_sampler);
   
   if (!AiStateGetMsgPtr("agsb_sampler", (void**)&sampler) || !sampler)
   {
      sg->out.VEC = AI_V3_ZERO;
   }
   else
   {
      float sample = 0.0f;
      
      CombineMode combine = (CombineMode) AiShaderEvalParamInt(p_combine);
      
      sg->out.VEC = AI_V3_ZERO;
      
      AtSamplerIterator *it = AiSamplerIterator(sampler, sg);
      
      while (AiSamplerGetSample(it, &sample))
      {
         AiStateSetMsgFlt("agsb_sample_value", sample);
         
         AtVector input = AiShaderEvalParamVec(p_input);
         
         switch (combine)
         {
         case CM_Max:
            sg->out.VEC.x = MAX(input.x, sg->out.VEC.x);
            sg->out.VEC.y = MAX(input.y, sg->out.VEC.y);
            sg->out.VEC.z = MAX(input.z, sg->out.VEC.z);
            break;
         case CM_Min:
            sg->out.VEC.x = MIN(input.x, sg->out.VEC.x);
            sg->out.VEC.y = MIN(input.y, sg->out.VEC.y);
            sg->out.VEC.z = MIN(input.z, sg->out.VEC.z);
            break;
         case CM_Add:
         case CM_Avg:
         default:
            sg->out.VEC += input;
            break;
         }
      }
      
      if (combine == CM_Avg)
      {
         float nrm = AiSamplerGetSampleInvCount(it);
         sg->out.VEC.x *= nrm;
         sg->out.VEC.y *= nrm;
         sg->out.VEC.z *= nrm;
      }
   }
}
