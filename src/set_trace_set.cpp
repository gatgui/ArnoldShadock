#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SetTraceSetMtd);

enum SetTraceSetParams
{
   p_input = 0,
   p_trace_set,
   p_inclusive
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterStr(SSTR::trace_set, "");
   AiParameterBool(SSTR::inclusive, true);
}

struct SetTraceSetData
{
   AtString traceSet;
   bool inclusive;
};

node_initialize
{
   AiNodeSetLocalData(node, new SetTraceSetData());
   AddMemUsage<SetTraceSetData>();
}

node_update
{
   SetTraceSetData *data = (SetTraceSetData*) AiNodeGetLocalData(node);
   data->traceSet = AiNodeGetStr(node, SSTR::trace_set);
   data->inclusive = AiNodeGetBool(node, SSTR::inclusive);
}

node_finish
{
   SetTraceSetData *data = (SetTraceSetData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<SetTraceSetData>();
}

shader_evaluate
{
   SetTraceSetData *data = (SetTraceSetData*) AiNodeGetLocalData(node);
   
   AiShaderGlobalsSetTraceSet(sg, data->traceSet, data->inclusive);
   
   sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   
   AiShaderGlobalsUnsetTraceSet(sg);
}
