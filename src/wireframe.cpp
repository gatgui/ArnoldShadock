#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WireframeMtd);

enum WireframeParams
{
   p_line_width = 0,
   p_width_space,
   p_edge_type
};

enum EdgeType
{
   ET_triangles = 0,
   ET_polygons,
   ET_patches
};

enum WidthSpace
{
   WS_world = 0,
   WS_screen
};

static const char* EdgeTypeNames[] = {"triangles", "polygons", "patches", NULL};

static const char* WidthSpaceNames[] = {"world", "screen", NULL};

node_parameters
{
   AiParameterFlt("line_width", 1.0f);
   AiParameterEnum("units", WS_screen, WidthSpaceNames);
   AiParameterEnum("edge_type", ET_polygons, EdgeTypeNames);
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
   float line_width = AiShaderEvalParamFlt(p_line_width);
   bool raster_space = (AiShaderEvalParamInt(p_width_space) == WS_screen);
   int edge_type = AiShaderEvalParamInt(p_edge_type);
   
   sg->out.FLT = AiWireframe(sg, line_width, raster_space, edge_type);
}
