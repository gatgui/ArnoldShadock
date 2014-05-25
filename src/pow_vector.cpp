#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(PowVectorMtd);

enum PowVectorParams
{
   p_input1,
   p_input2
};

node_parameters
{
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 1.0f, 1.0f, 1.0f);
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
   
   sg->out.VEC.x = powf(input1.x, input2.x);
   sg->out.VEC.y = powf(input1.y, input2.y);
   sg->out.VEC.z = powf(input1.z, input2.z);
}
