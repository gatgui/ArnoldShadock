#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleLoopC3Mtd);

enum SampleLoopC3Params
{
   p_sampler = 0,
   p_input,
   p_combine
};

node_parameters
{
   AiParameterRGB("sampler", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::combine, CM_Add, CombineModeNames);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *data = (int*) AiNodeGetLocalData(node);
   *data = AiNodeGetInt(node, SSTR::combine);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

shader_evaluate
{
   AtSampler *sampler = 0;
   
   AiShaderEvalParamRGB(p_sampler);
   
   if (!AiStateGetMsgPtr(SSTR::agsb_sampler, (void**)&sampler) || !sampler)
   {
      sg->out.RGB = AI_RGB_BLACK;
   }
   else
   {
      AtPoint2 sample;
      
      CombineMode combine = (CombineMode) *((int*) AiNodeGetLocalData(node));
      
      sg->out.RGB = AI_RGB_BLACK;
      
      AtSamplerIterator *it = AiSamplerIterator(sampler, sg);
      
      while (AiSamplerGetSample(it, &(sample.x)))
      {
         AiStateSetMsgPnt2(SSTR::agsb_sample_value, sample);
         
         AtColor input = AiShaderEvalParamRGB(p_input);
         
         switch (combine)
         {
         case CM_Max:
            sg->out.RGB.r = MAX(input.r, sg->out.RGB.r);
            sg->out.RGB.g = MAX(input.g, sg->out.RGB.g);
            sg->out.RGB.b = MAX(input.b, sg->out.RGB.b);
            break;
         case CM_Min:
            sg->out.RGB.r = MIN(input.r, sg->out.RGB.r);
            sg->out.RGB.g = MIN(input.g, sg->out.RGB.g);
            sg->out.RGB.b = MIN(input.b, sg->out.RGB.b);
            break;
         case CM_Add:
         case CM_Avg:
         default:
            sg->out.RGB += input;
            break;
         }
      }
      
      if (combine == CM_Avg)
      {
         float nrm = AiSamplerGetSampleInvCount(it);
         sg->out.RGB.r *= nrm;
         sg->out.RGB.g *= nrm;
         sg->out.RGB.b *= nrm;
      }
   }
}
