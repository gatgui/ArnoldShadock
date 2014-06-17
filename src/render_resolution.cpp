#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RenderResolutionMtd);

struct NodeData
{
   float w;
   float h;
};

node_parameters
{
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   AtNode *opts = AiUniverseGetOptions();
   data->w = float(AiNodeGetInt(opts, "xres"));
   data->h = float(AiNodeGetInt(opts, "yres"));
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   sg->out.VEC.x = float(data->w);
   sg->out.VEC.y = float(data->h);
   sg->out.VEC.z = (sg->out.VEC.y >= AI_EPSILON ? (sg->out.VEC.x / sg->out.VEC.y) : 0.0f);
}

