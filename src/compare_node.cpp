#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(CompareNodeMtd);

enum CompareNodeParams
{
   p_node1 = 0,
   p_node2
};

node_parameters
{
   AiParameterNode("node1", 0);
   AiParameterNode("node2", 0);
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
   void *ptr1 = AiShaderEvalParamPtr(p_node1);
   void *ptr2 = AiShaderEvalParamPtr(p_node2);
   
   sg->out.BOOL = (ptr1 == ptr2);
}
