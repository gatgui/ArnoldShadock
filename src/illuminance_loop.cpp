#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(IlluminanceLoopMtd);

enum IlluminanceLoopParams
{
   p_input = 0,
   p_combine,
   p_P,
   p_P_space,
   p_N,
   p_N_space,
   p_no_shadows,
   p_trace_set,
   p_exclusive_trace_set,
   p_hemispherical
};

enum Combine
{
   C_Add = 0,
   C_Avg,
   C_Max,
   C_Min
};

static const char* CombineNames[] =
{
   "add",
   "avg",
   "max",
   "min",
   NULL
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("combine", C_Add, CombineNames);
   AiParameterPnt("P", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("P_space", CS_World, CoordinateSpaceNames);
   AiParameterVec("N", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("N_space", CS_World, CoordinateSpaceNames);
   AiParameterBool("no_shadows", false);
   AiParameterStr("trace_set", "");
   AiParameterBool("exclusive_trace_set", false);
   AiParameterBool("hemispherical", true);
   
   AiMetaDataSetBool(mds, "combine", "linkable", false);
   AiMetaDataSetBool(mds, "P_space", "linkable", false);
   AiMetaDataSetBool(mds, "N_space", "linkable", false);
   AiMetaDataSetBool(mds, "no_shadows", "linkable", false);
   AiMetaDataSetBool(mds, "trace_set", "linkable", false);
   AiMetaDataSetBool(mds, "exclusive_trace_set", "linkable", false);
   AiMetaDataSetBool(mds, "hemispherical", "linkable", false);
}

struct NodeData
{
   Combine combine;
   bool P_is_linked;
   CoordinateSpace P_space;
   bool N_is_linked;
   CoordinateSpace N_space;
   bool no_shadows;
   const char *trace_set;
   bool exclusive_trace_set;
   bool hemispherical;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->combine = (Combine) AiNodeGetInt(node, "combine");
   data->P_is_linked = AiNodeIsLinked(node, "P");
   data->P_space = (CoordinateSpace) AiNodeGetInt(node, "P_space");
   data->N_is_linked = AiNodeIsLinked(node, "N");
   data->N_space = (CoordinateSpace) AiNodeGetInt(node, "N_space");
   data->no_shadows = AiNodeGetBool(node, "no_shadows");
   data->trace_set = AiNodeGetStr(node, "trace_set");
   data->exclusive_trace_set = AiNodeGetBool(node, "exclusive_trace_set");
   data->hemispherical = AiNodeGetBool(node, "hemispherical");
   
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
   bool old_skip_shadow = sg->skip_shadow;
   bool old_fhemi = sg->fhemi;
   
   if (data->P_is_linked)
   {
      if (data->P_space == CS_World)
      {
         sg->P = AiShaderEvalParamPnt(p_P);
         AiM4PointByMatrixMult(&(sg->Po), sg->Minv, &(sg->P));
      }
      else
      {
         sg->Po = AiShaderEvalParamPnt(p_P);
         AiM4PointByMatrixMult(&(sg->P), sg->M, &(sg->Po));
      }
      reset_lights_cache = true;
   }
   
   if (data->N_is_linked)
   {
      sg->N = AiShaderEvalParamVec(p_N);
      if (data->N_space == CS_Object)
      {
         AtVector No = sg->N;
         AiM4VectorByMatrixMult(&(sg->N), sg->M, &No);
         sg->N = AiV3Normalize(sg->N);
      }
      sg->Ns = sg->N;
      AiFaceForward(&(sg->Nf), sg->Rd);
      reset_lights_cache = true;
   }
   
   if (data->trace_set)
   {
      AiShaderGlobalsSetTraceSet(sg, data->trace_set, !data->exclusive_trace_set);
      reset_lights_cache = true;
   }
   
   sg->skip_shadow = data->no_shadows;
   sg->fhemi = data->hemispherical;
   
   if (reset_lights_cache)
   {
      AiLightsResetCache(sg);
   }
   
   AiLightsPrepare(sg);
   
   sg->out.RGB = AI_RGB_BLACK;
   
   float sample_count = 0.0f;
   AtColor sample_value;
   
   while (AiLightsGetSample(sg))
   {
      sample_value = AiShaderEvalParamRGB(p_input);
      switch (data->combine)
      {
      case C_Max:
         sg->out.RGB.r = MAX(sample_value.r, sg->out.RGB.r);
         sg->out.RGB.g = MAX(sample_value.g, sg->out.RGB.g);
         sg->out.RGB.b = MAX(sample_value.b, sg->out.RGB.b);
         break;
      case C_Min:
         sg->out.RGB.r = MIN(sample_value.r, sg->out.RGB.r);
         sg->out.RGB.g = MIN(sample_value.g, sg->out.RGB.g);
         sg->out.RGB.b = MIN(sample_value.b, sg->out.RGB.b);
         break;
      case C_Add:
      case C_Avg:
      default:
         sg->out.RGB += sample_value;
      }
      
      sample_count += 1.0f;
   }
   
   if (data->combine == C_Avg && sample_count > 0.0f)
   {
      sample_count = 1.0f / sample_count;
      sg->out.RGB *= sample_count;
   }
   
   sg->fhemi = old_fhemi;
   sg->skip_shadow = old_skip_shadow;
   
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
