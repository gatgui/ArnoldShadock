#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleLoopVMtd);

enum SampleLoopVParams
{
   p_sampler = 0,
   p_input,
   p_combine
};

node_parameters
{
   AiParameterRGB("sampler", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::combine, CM_Add, CombineModeNames);
   
   AiMetaDataSetBool(mds, SSTR::combine, SSTR::linkable, false);
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
      sg->out.VEC = AI_V3_ZERO;
   }
   else
   {
      AtPoint2 sample;
      
      CombineMode combine = (CombineMode) *((int*) AiNodeGetLocalData(node));
      
      sg->out.VEC = AI_V3_ZERO;
      
      AtSamplerIterator *it = AiSamplerIterator(sampler, sg);
      
      while (AiSamplerGetSample(it, &(sample.x)))
      {
         AiStateSetMsgPnt2(SSTR::agsb_sample_value, sample);
         
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
