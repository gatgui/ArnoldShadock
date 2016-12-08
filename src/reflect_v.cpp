#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ReflectVMtd);

enum ReflectVParams
{
   p_direction = 0,
   p_normal,
   p_safe,
   p_geometric_normal
};

node_parameters
{
   AiParameterVec(SSTR::direction, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::normal, 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::safe, false);
   AiParameterVec(SSTR::geometric_normal, 0.0f, 0.0f, 0.0f);
}

struct ReflectVData
{
   bool evalN;
   bool safe;
   bool evalNg;
};

node_initialize
{
   AiNodeSetLocalData(node, new ReflectVData());
   AddMemUsage<ReflectVData>();
}

node_update
{
   ReflectVData *data = (ReflectVData*) AiNodeGetLocalData(node);
   
   data->evalN = AiNodeIsLinked(node, SSTR::normal);
   data->evalNg = AiNodeIsLinked(node, SSTR::geometric_normal);
   data->safe = AiNodeGetBool(node, SSTR::safe);
}

node_finish
{
   ReflectVData *data = (ReflectVData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ReflectVData>();
}

shader_evaluate
{
   ReflectVData *data = (ReflectVData*) AiNodeGetLocalData(node);
   
   AtVector D = AiShaderEvalParamVec(p_direction);
   AtVector N = (data->evalN ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   if (data->safe)
   {
      AtVector Ng = (data->evalNg ? AiShaderEvalParamVec(p_geometric_normal) : sg->Ng);
      AiReflectSafe(&D, &N, &Ng, &(sg->out.VEC));
   }
   else
   {
      AiReflect(&D, &N, &(sg->out.VEC));
   }
}
