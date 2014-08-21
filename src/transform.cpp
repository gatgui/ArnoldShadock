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
   AiParameterMtx("matrix", id);
   AiParameterVec("vector", 0.0f, 0.0f, 0.0f);
   AiParameterBool("as_point", false);
   
   AiMetaDataSetBool(mds, "as_point", "linkable", false);
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
   AtMatrix *mtx = AiShaderEvalParamMtx(p_matrix);
   AtVector vec = AiShaderEvalParamVec(p_vector);

   if (AiShaderEvalParamBool(p_as_point))
   {
      AiM4PointByMatrixMult(&(sg->out.PNT), *mtx, &vec);
   }
   else
   {
      AiM4VectorByMatrixMult(&(sg->out.VEC), *mtx, &vec);
   }
}
