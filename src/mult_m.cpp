#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MultiplyMatrixMtd);

enum MultiplyMatrixParams
{
   p_input1 = 0,
   p_input2
};

node_parameters
{
   AtMatrix id;
   AiM4Identity(id);
   
   AiParameterMtx("input1", id);
   AiParameterMtx("input2", id);
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
   AtMatrix *mtx1 = AiShaderEvalParamMtx(p_input1);
   AtMatrix *mtx2 = AiShaderEvalParamMtx(p_input2);
   
   sg->out.pMTX = (AtMatrix*) AiShaderGlobalsQuickAlloc(sg, sizeof(AtMatrix));
   
   AiM4Mult(*(sg->out.pMTX), *mtx1, *mtx2);
}
