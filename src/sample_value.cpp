#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleValueMtd);

enum SampleValueParams
{
   p_default = 0
};

node_parameters
{
   AiParameterPnt2("default", 0.0f, 0.0f);
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
   AtPoint2 sample;
   
   if (!AiStateGetMsgPnt2(SSTR::agsb_sample_value, &sample))
   {
      sg->out.PNT2 = AiShaderEvalParamPnt2(p_default);
   }
   else
   {
      sg->out.PNT2 = sample;
   }
}
