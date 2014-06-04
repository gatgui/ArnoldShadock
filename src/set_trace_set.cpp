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
   AiParameterStr("trace_set", "");
   AiParameterBool("inclusive", true);
   
   AiMetaDataSetBool(mds, "trace_set", "linkable", false);
   AiMetaDataSetBool(mds, "inclusive", "linkable", false);
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
   AiShaderGlobalsSetTraceSet(sg, AiShaderEvalParamStr(p_trace_set), AiShaderEvalParamBool(p_inclusive));
   sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   AiShaderGlobalsUnsetTraceSet(sg);
}
