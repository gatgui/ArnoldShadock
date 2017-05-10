/*
MIT License
Copyright (c) 2014~ Gaetan Guidet
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
