#include "common.h"
#include <gmath/color.h>

AI_SHADER_NODE_EXPORT_METHODS(BlackbodyMtd);

enum BlackbodyParams
{
   p_temperature = 0,
   p_normalize,
   p_color_space,
   
   // remap ramp?
};

node_parameters
{
   AiParameterFlt("temperature", 3000.0f);
   AiParameterBool("normalize", false); // mode: raw, normalized, remap ?
   AiParameterEnum("color_space", CS_Rec790, ColorSpaceNames);

   AiMetaDataSetBool(mds, "normalize", "linkable", false);
   AiMetaDataSetBool(mds, "color_space", "linkable", false);
}

struct NodeData
{
   bool normalize;
   const gmath::ColorSpace *CS;
};

node_initialize
{
   NodeData *data = new NodeData();
   AiNodeSetLocalData(node, (void*)data);
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   data->normalize = AiNodeGetBool(node, "normalize");
   data->CS = ColorSpaces[AiNodeGetInt(node, "color_space")];
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   float temperature = AiShaderEvalParamFlt(p_temperature);
   
   gmath::RGB out = gmath::Blackbody::GetRGB(temperature, *(data->CS), data->normalize);

   sg->out.RGB.r = out.r;
   sg->out.RGB.g = out.g;
   sg->out.RGB.b = out.b;
}
