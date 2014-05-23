#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ConvertFloatToIntMtd);

enum ConvertFloatToIntParams
{
   p_input = 0,
   p_mode
};

static const char* ConvertFloatToIntModeNames[] =
{
   "floor",
   "ceil",
   "round",
   "truncate",
   NULL
};

enum ConvertFloatToIntMode
{
   FTI_FLOOR = 0,
   FTI_CEIL,
   FTI_ROUND,
   FTI_TRUNCATE
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterEnum("mode", 0, ConvertFloatToIntModeNames);
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
   ConvertFloatToIntMode mode = (ConvertFloatToIntMode) AiShaderEvalParamInt(p_mode);
   float input = AiShaderEvalParamFlt(p_input);
   
   switch (mode)
   {
   case FTI_CEIL:
      sg->out.INT = static_cast<int>(ceilf(input));
      return;
   case FTI_ROUND:
      sg->out.INT = static_cast<int>(floorf(0.5f + input));
      return;
   case FTI_TRUNCATE:
      sg->out.INT = static_cast<int>(input < 0.0f ? -floorf(-input) : floorf(input));
      return;
   case FTI_FLOOR:
   default:
      sg->out.INT = static_cast<int>(floorf(input));
      return;
   }
}
