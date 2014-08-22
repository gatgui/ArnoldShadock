#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RefractVMtd);

enum RefractVParams
{
   p_direction = 0,
   p_normal,
   p_ior_1,
   p_ior_2
};

node_parameters
{
   AiParameterVec("direction", 0.0f, 0.0f, 0.0f);
   AiParameterVec("normal", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("ior1", 1.0f);
   AiParameterFlt("ior2", 1.0f);
   
   AiMetaDataSetBool(mds, "ior1", "linkable", false);
   AiMetaDataSetBool(mds, "ior2", "linkable", false);
}

struct RefractVData
{
   bool N_is_linked;
   float ior1;
   float ior2;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(RefractVData)));
}

node_update
{
   RefractVData *data = (RefractVData*) AiNodeGetLocalData(node);
   
   data->N_is_linked = AiNodeIsLinked(node, "normal");
   data->ior1 = AiNodeGetFlt(node, "ior1");
   data->ior2 = AiNodeGetFlt(node, "ior1");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   RefractVData *data = (RefractVData*) AiNodeGetLocalData(node);
   
   AtVector D = AiShaderEvalParamVec(p_direction);
   AtVector N = (data->N_is_linked ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   if (!AiRefract(&D, &N, &(sg->out.VEC), data->ior1, data->ior2))
   {
      // Total Internal Reflection
      sg->out.VEC = AI_V3_ZERO;
   }
}
