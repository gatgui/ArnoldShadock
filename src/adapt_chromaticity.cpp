#include "common.h"
#include <gmath/color.h>

AI_SHADER_NODE_EXPORT_METHODS(AdaptChromaticityMtd);

enum AdaptChromaticityParams
{
   p_input = 0,
   p_source,
   p_source_cs,
   p_destination,
   p_destination_cs,
   p_cat
};

//GMATH_API Matrix3 ChromaticAdaptationMatrix(const XYZ &from, const XYZ &to, ChromaticAdaptationTransform cat=CAT_VonKries);

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);

   AiParameterRGB("source", 1.0f, 1.0f, 1.0f);
   AiParameterEnum("source_color_space", CS_Rec790, ColorSpaceNames);
   AiParameterRGB("destination", 1.0f, 1.0f, 1.0f);
   AiParameterEnum("destination_color_space", CS_Rec790, ColorSpaceNames);
   AiParameterEnum("transform", gmath::CAT_VonKries, ChromaticAdaptationTransformNames);

   AiMetaDataSetBool(mds, "source", "linkable", false);
   AiMetaDataSetBool(mds, "source_color_space", "linkable", false);
   AiMetaDataSetBool(mds, "destination", "linkable", false);
   AiMetaDataSetBool(mds, "destination_color_space", "linkable", false);
   AiMetaDataSetBool(mds, "transform", "linkable", false);
}

struct NodeData
{
   gmath::Matrix3 M;
};

node_initialize
{
   NodeData *data = new NodeData();
   AiNodeSetLocalData(node, (void*)data);
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   AtRGB src = AiNodeGetRGB(node, "source");
   AtRGB dst = AiNodeGetRGB(node, "destination");
   const gmath::ColorSpace *srcCS = ColorSpaces[AiNodeGetInt(node, "source_color_space")];
   const gmath::ColorSpace *dstCS = ColorSpaces[AiNodeGetInt(node, "destination_color_space")];
   gmath::ChromaticAdaptationTransform cat = (gmath::ChromaticAdaptationTransform) AiNodeGetInt(node, "transform");

   gmath::XYZ _src = srcCS->RGBtoXYZ(gmath::RGB(src.r, src.g, src.b));
   gmath::XYZ _dst = dstCS->RGBtoXYZ(gmath::RGB(dst.r, dst.g, dst.b));
   data->M = dstCS->getXYZtoRGBMatrix() * gmath::ChromaticAdaptationMatrix(_src, _dst, cat) * srcCS->getRGBtoXYZMatrix();
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   AtRGB input = AiShaderEvalParamRGB(p_input);

   gmath::Vector3 out = data->M * gmath::Vector3(input.r, input.g, input.b);

   sg->out.RGB.r = out.x;
   sg->out.RGB.g = out.y;
   sg->out.RGB.b = out.z;
}
