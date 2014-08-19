#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleValueMtd);

enum SampleValueParams
{
   p_default = 0
};

node_parameters
{
   AiParameterFlt("default", 0.0f);
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
   float sample = 0.0f;
   
   if (!AiStateGetMsgFlt("agsb_sample_value", &sample))
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default);
   }
   else
   {
      sg->out.FLT = sample;
   }
}
