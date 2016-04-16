#include "common.h"
#include <gmath/color.h>

AI_SHADER_NODE_EXPORT_METHODS(AdaptChromaticityMtd);

enum AdaptChromaticityParams
{
   p_input = 0,
   p_source,
   p_source_cs,
   p_target,
   p_target_cs,
   p_transform
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);

   AiParameterRGB(SSTR::source, 1.0f, 1.0f, 1.0f);
   AiParameterEnum(SSTR::source_color_space, CS_Rec709, ColorSpaceNames);
   AiParameterRGB(SSTR::target, 1.0f, 1.0f, 1.0f);
   AiParameterEnum(SSTR::target_color_space, CS_Rec709, ColorSpaceNames);
   AiParameterEnum(SSTR::transform, gmath::CAT_VonKries, ChromaticAdaptationTransformNames);

   AiMetaDataSetBool(mds, SSTR::source, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::source_color_space, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::target, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::target_color_space, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::transform, SSTR::linkable, false);
}

struct NodeData
{
   gmath::Matrix3 M;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   AtRGB src = AiNodeGetRGB(node, SSTR::source);
   AtRGB tgt = AiNodeGetRGB(node, SSTR::target);
   const gmath::ColorSpace *srcCS = ColorSpaces[AiNodeGetInt(node, SSTR::source_color_space)];
   const gmath::ColorSpace *tgtCS = ColorSpaces[AiNodeGetInt(node, SSTR::target_color_space)];
   gmath::ChromaticAdaptationTransform cat = (gmath::ChromaticAdaptationTransform) AiNodeGetInt(node, SSTR::transform);

   gmath::XYZ _src = srcCS->RGBtoXYZ(gmath::RGB(src.r, src.g, src.b));
   gmath::XYZ _tgt = tgtCS->RGBtoXYZ(gmath::RGB(tgt.r, tgt.g, tgt.b));
   data->M = tgtCS->getXYZtoRGBMatrix() * gmath::ChromaticAdaptationMatrix(_src, _tgt, cat) * srcCS->getRGBtoXYZMatrix();
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
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
