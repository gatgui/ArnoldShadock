#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(LocalFrameMtd);

enum LocalFrameParams
{
   p_method = 0,
   p_N,
   p_output_vector
};

enum Method
{
   M_polar = 0,
   M_shirley
};

enum OutputVector
{
   OV_u = 0,
   OV_v
};

const char* MethodNames[] = { "polar", "shirley", NULL };

const char* OutputVectorNames[] = { "u", "v", NULL };

node_parameters
{
   AiParameterEnum("method", M_polar, MethodNames);
   AiParameterVec("N", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("output_vector", OV_u, OutputVectorNames);
   
   AiMetaDataSetBool(mds, "method", "linkable", false);
   AiMetaDataSetBool(mds, "output_vector", "linkable", false);
}

struct NodeData
{
   Method method;
   bool N_is_linked;
   OutputVector output_vector;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->method = (Method) AiNodeGetInt(node, "method");
   data->N_is_linked = AiNodeIsLinked(node, "N");
   data->output_vector = (OutputVector) AiNodeGetInt(node, "output_vector");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   AtVector U, V, N = (data->N_is_linked ? AiShaderEvalParamVec(p_N) : sg->N);
   
   switch (data->method)
   {
   case M_shirley:
      AiBuildLocalFrameShirley(&U, &V, &N);
      break;
   case M_polar:
   default:
      AiBuildLocalFramePolar(&U, &V, &N);
      break;
   }
   
   switch (data->output_vector)
   {
   case OV_v:
      sg->out.VEC = V;
      break;
   case OV_u:
   default:
      sg->out.VEC = U;
      break;
   }
}
