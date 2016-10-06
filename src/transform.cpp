#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TransformMtd);

enum TransformParams
{
   p_matrix = 0,
   p_vector,
   p_as_point
};

node_parameters
{
   AtMatrix id;
   AiM4Identity(id);
   AiParameterMtx(SSTR::matrix, id);
   AiParameterVec("vector", 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::as_point, false);
}

struct NodeData
{
   bool evalMatrix;
   AtMatrix matrix;
   bool asPoint;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   data->asPoint = AiNodeGetBool(node, SSTR::as_point);
   data->evalMatrix = AiNodeIsLinked(node, SSTR::matrix);
   if (!data->evalMatrix)
   {
      AiNodeGetMatrix(node, SSTR::matrix, data->matrix);
   }
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   AtMatrix *mtx = (data->evalMatrix ? AiShaderEvalParamMtx(p_matrix) : &(data->matrix));
   AtVector vec = AiShaderEvalParamVec(p_vector);

   if (data->asPoint)
   {
      AiM4PointByMatrixMult(&(sg->out.PNT), *mtx, &vec);
   }
   else
   {
      AiM4VectorByMatrixMult(&(sg->out.VEC), *mtx, &vec);
   }
}
