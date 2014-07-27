#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(IlluminanceLoopMtd);

enum IlluminanceLoopParams
{
   p_input = 0,
   p_position,
   p_normal,
   p_trace_set,
   p_exclusive_trace_set
   //p_reset_lights_cache
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterPnt("position", 0.0f, 0.0f, 0.0f);
   AiParameterVec("normal", 0.0f, 0.0f, 0.0f);
   AiParameterStr("trace_set", "");
   AiParameterBool("exclusive_trace_set", false);
   
   AiMetaDataSetBool(mds, "trace_set", "linkable", false);
   AiMetaDataSetBool(mds, "exclusive_trace_set", "linkable", false);
}

struct NodeData
{
   bool P_is_linked;
   bool N_is_linked;
   const char *trace_set;
   bool exclusive_trace_set;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->P_is_linked = AiNodeIsLinked(node, "position");
   data->N_is_linked = AiNodeIsLinked(node, "normal");
   data->trace_set = AiNodeGetStr(node, "trace_set");
   data->exclusive_trace_set = AiNodeGetBool(node, "exclusive_trace_set");
   
   if (data->trace_set && data->trace_set[0] == '\0')
   {
      data->trace_set = 0;
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   bool reset_lights_cache = false;
   
   AtPoint old_P = sg->P;
   AtPoint old_Po = sg->Po;
   AtVector old_N = sg->N;
   AtVector old_Nf = sg->Nf;
   AtVector old_Ns = sg->Ns;
   const char *old_traceset = sg->traceset;
   bool old_inclusive_traceset = sg->inclusive_traceset;
   
   if (data->P_is_linked)
   {
      sg->P = AiShaderEvalParamPnt(p_position);
      AiM4PointByMatrixMult(&(sg->Po), sg->Minv, &(sg->P));
      reset_lights_cache = true;
   }
   
   if (data->N_is_linked)
   {
      sg->N = AiShaderEvalParamVec(p_normal);
      sg->Ns = sg->N;
      AiFaceForward(&(sg->Nf), sg->Rd);
      reset_lights_cache = true;
   }
   
   if (data->trace_set)
   {
      AiShaderGlobalsSetTraceSet(sg, data->trace_set, !data->exclusive_trace_set);
      reset_lights_cache = true;
   }
   
   if (reset_lights_cache)
   {
      AiLightsResetCache(sg);
   }
   
   AiLightsPrepare(sg);
   
   sg->out.RGB = AI_RGB_BLACK;
   
   while (AiLightsGetSample(sg))
   {
      sg->out.RGB += AiShaderEvalParamRGB(p_input);
   }
   
   if (data->P_is_linked)
   {
      sg->P = old_P;
      sg->Po = old_Po;
   }
   
   if (data->N_is_linked)
   {
      sg->N = old_N;
      sg->Ns = old_Ns;
      sg->Nf = old_Nf;
   }
   
   if (data->trace_set)
   {
      if (!old_traceset || old_traceset[0] == '\0')
      {
         AiShaderGlobalsUnsetTraceSet(sg);
      }
      else
      {
         AiShaderGlobalsSetTraceSet(sg, old_traceset, old_inclusive_traceset);
      }
   }
}
