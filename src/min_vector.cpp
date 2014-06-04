#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MinVectorMtd);

enum MinVectorParams
{
   p_input1 = 0,
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
   
   sg->out.VEC.x = MIN(input1.x, input2.x);
   sg->out.VEC.y = MIN(input1.y, input2.y);
   sg->out.VEC.z = MIN(input1.z, input2.z);
}
