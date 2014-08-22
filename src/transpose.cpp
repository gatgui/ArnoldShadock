#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(TransposeMtd);

enum TransposeParams
{
   p_input = 0
};

node_parameters
{
   AtMatrix id;
   AiM4Identity(id);
   
   AiParameterMtx("input", id);
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
   sg->out.pMTX = (AtMatrix*) AiShaderGlobalsQuickAlloc(sg, sizeof(AtMatrix));
   
   AtMatrix *mtx = AiShaderEvalParamMtx(p_input);
   AiM4Transpose(*mtx, *(sg->out.pMTX));
}
