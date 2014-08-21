#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SamplesLoopFloatMtd);

enum SamplesLoopFloatParams
{
   p_sampler = 0,
   p_input,
   p_combine
};

node_parameters
{
   AiParameterRGB("sampler", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("input", 0.0f);
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
      sg->out.RGB = AI_RGB_BLACK;
   }
   else
   {
      AtPoint2 sample;
      
      CombineMode combine = (CombineMode) AiShaderEvalParamInt(p_combine);
      
      sg->out.FLT = 0.0f;
      
      AtSamplerIterator *it = AiSamplerIterator(sampler, sg);
      
      while (AiSamplerGetSample(it, &(sample.x)))
      {
         AiStateSetMsgPnt2("agsb_sample_value", sample);
         
         float input = AiShaderEvalParamFlt(p_input);
         
         switch (combine)
         {
         case CM_Max:
            sg->out.FLT = MAX(input, sg->out.FLT);
            break;
         case CM_Min:
            sg->out.FLT = MIN(input, sg->out.FLT);
            break;
         case CM_Add:
         case CM_Avg:
         default:
            sg->out.FLT += input;
            break;
         }
      }
      
      if (combine == CM_Avg)
      {
         sg->out.FLT *= AiSamplerGetSampleInvCount(it);
      }
   }
}
