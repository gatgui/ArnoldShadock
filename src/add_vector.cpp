#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(AddVectorMtd);

enum AddVectorParams
{
   p_input1,
   p_input2
};

node_parameters
{
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 0.0f, 0.0f, 0.0f);
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
   AtVector input1 = AiShaderEvalParamVec(p_input1);
   AtVector input2 = AiShaderEvalParamVec(p_input2);
   
   sg->out.VEC = input1 + input2;
}
