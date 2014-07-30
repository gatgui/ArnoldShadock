#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReflectMtd);

enum ReflectParams
{
   p_direction = 0,
   p_normal,
   p_safe,
   p_geometric_normal
};

node_parameters
{
   AiParameterVec("direction", 0.0f, 0.0f, 0.0f);
   AiParameterVec("normal", 0.0f, 0.0f, 0.0f);
   AiParameterBool("safe", false);
   AiParameterVec("geometric_normal", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "safe", "linkable", false);
}

struct ReflectData
{
   bool N_is_linked;
   bool safe;
   bool Ng_is_linked;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(ReflectData)));
}

node_update
{
   ReflectData *data = (ReflectData*) AiNodeGetLocalData(node);
   
   data->N_is_linked = AiNodeIsLinked(node, "normal");
   data->Ng_is_linked = AiNodeIsLinked(node, "geometric_normal");
   data->safe = AiNodeGetBool(node, "safe");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   ReflectData *data = (ReflectData*) AiNodeGetLocalData(node);
   
   AtVector D = AiShaderEvalParamVec(p_direction);
   AtVector N = (data->N_is_linked ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   if (data->safe)
   {
      AtVector Ng = (data->Ng_is_linked ? AiShaderEvalParamVec(p_geometric_normal) : sg->Ng);
      AiReflectSafe(&D, &N, &Ng, &(sg->out.VEC));
   }
   else
   {
      AiReflect(&D, &N, &(sg->out.VEC));
   }
}
