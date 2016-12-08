#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WireframeMtd);

enum WireframeParams
{
   p_line_width = 0,
   p_units,
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
   AiParameterEnum(SSTR::units, WS_screen, WidthSpaceNames);
   AiParameterEnum(SSTR::edge_type, ET_polygons, EdgeTypeNames);
}

struct WireframeData
{
   int units;
   int edgeType;
};

node_initialize
{
   AiNodeSetLocalData(node, new WireframeData());
   AddMemUsage<WireframeData>();
}

node_update
{
   WireframeData *data = (WireframeData*) AiNodeGetLocalData(node);
   data->units = AiNodeGetInt(node, SSTR::units);
   data->edgeType = AiNodeGetInt(node, SSTR::edge_type);
}

node_finish
{
   WireframeData *data = (WireframeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<WireframeData>();
}

shader_evaluate
{
   WireframeData *data = (WireframeData*) AiNodeGetLocalData(node);

   float lineWidth = AiShaderEvalParamFlt(p_line_width);
   
   sg->out.FLT = AiWireframe(sg, lineWidth, data->units == WS_screen, data->edgeType);
}
