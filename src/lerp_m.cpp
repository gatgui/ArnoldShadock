#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(LerpMMtd);

enum LerpMParams
{
   p_input1 = 0,
   p_input2,
   p_blend
};

node_parameters
{
   AtMatrix id;
   AiM4Identity(id);
   
   AiParameterMtx("input1", id);
   AiParameterMtx("input2", id);
   AiParameterFlt("blend", 0.0f);
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
   
   AtMatrix *mtx1 = AiShaderEvalParamMtx(p_input1);
   AtMatrix *mtx2 = AiShaderEvalParamMtx(p_input2);
   float blend = AiShaderEvalParamFlt(p_blend);
   
   AiM4Lerp(*(sg->out.pMTX), blend, *mtx1, *mtx2);
}
