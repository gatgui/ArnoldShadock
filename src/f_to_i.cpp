#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(FToIMtd);

enum FToIParams
{
   p_input = 0,
   p_mode
};

static const char* FloatToIntModeNames[] =
{
   "floor",
   "ceil",
   "round",
   "truncate",
   NULL
};

enum FloatToIntMode
{
   FTI_FLOOR = 0,
   FTI_CEIL,
   FTI_ROUND,
   FTI_TRUNCATE
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterEnum(SSTR::mode, 0, FloatToIntModeNames);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *data = (int*) AiNodeGetLocalData(node);
   *data = AiNodeGetInt(node, SSTR::mode);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

shader_evaluate
{
   FloatToIntMode mode = (FloatToIntMode) *((int*) AiNodeGetLocalData(node));
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
