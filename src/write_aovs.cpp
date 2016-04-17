#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WriteAOVsMtd);

enum WriteAOVsParams
{
   p_input = 0,
   p_bool_aov_name,
   p_bool_aov,
   p_int_aov_name,
   p_int_aov,
   p_float_aov_name,
   p_float_aov,
   p_point2_aov_name,
   p_point2_aov,
   p_point_aov_name,
   p_point_aov,
   p_vector_aov_name,
   p_vector_aov,
   p_rgb_aov_name,
   p_rgb_aov,
   p_rgba_aov_name,
   p_rgba_aov,
   p_blend_opacity
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterStr(SSTR::bool_aov_name, "");
   AiParameterBool("bool_aov", false);
   
   AiParameterStr(SSTR::int_aov_name, "");
   AiParameterInt("int_aov", 0);
   
   AiParameterStr(SSTR::float_aov_name, "");
   AiParameterFlt("float_aov", 0.0f);
   
   AiParameterStr(SSTR::point2_aov_name, "");
   AiParameterPnt2("point2_aov", 0.0f, 0.0f);
   
   AiParameterStr(SSTR::point_aov_name, "");
   AiParameterPnt("point_aov", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::vector_aov_name, "");
   AiParameterVec("vector_aov", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::rgb_aov_name, "");
   AiParameterRGB("rgb_aov", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::rgba_aov_name, "");
   AiParameterRGBA("rgba_aov", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterBool(SSTR::blend_opacity, true);
   
   AiMetaDataSetBool(mds, SSTR::bool_aov_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::int_aov_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::float_aov_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::point2_aov_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::point_aov_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::vector_aov_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::rgb_aov_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::rgba_aov_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::blend_opacity, SSTR::linkable, false);
}

struct NodeData
{
   bool hasBool;
   bool hasInt;
   bool hasFloat;
   bool hasPoint2;
   bool hasPoint;
   bool hasVector;
   bool hasRGB;
   bool hasRGBA;
   AtString boolName;
   AtString intName;
   AtString floatName;
   AtString point2Name;
   AtString pointName;
   AtString vectorName;
   AtString rgbName;
   AtString rgbaName;
};

node_initialize
{
   NodeData *data = new NodeData();
   AddMemUsage<NodeData>();
   
   data->hasBool = false;
   data->hasInt = false;
   data->hasFloat = false;
   data->hasPoint2 = false;
   data->hasPoint = false;
   data->hasVector = false;
   data->hasRGB = false;
   data->hasRGBA = false;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   bool blend = AiNodeGetBool(node, SSTR::blend_opacity);
   
   data->boolName = AiNodeGetStr(node, SSTR::bool_aov_name);
   if (!data->boolName.empty())
   {
      AiAOVRegister(data->boolName, AI_TYPE_BOOLEAN, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasBool = true;
   }
   
   data->intName = AiNodeGetStr(node, SSTR::int_aov_name);
   if (!data->intName.empty())
   {
      AiAOVRegister(data->intName, AI_TYPE_INT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasInt = true;
   }
   
   data->floatName = AiNodeGetStr(node, SSTR::float_aov_name);
   if (!data->floatName.empty())
   {
      AiAOVRegister(data->floatName, AI_TYPE_FLOAT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasFloat = true;
   }
   
   data->point2Name = AiNodeGetStr(node, SSTR::point2_aov_name);
   if (!data->point2Name.empty())
   {
      AiAOVRegister(data->point2Name, AI_TYPE_POINT2, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasPoint2 = true;
   }
   
   data->pointName = AiNodeGetStr(node, SSTR::point_aov_name);
   if (!data->pointName.empty())
   {
      AiAOVRegister(data->pointName, AI_TYPE_POINT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasPoint = true;
   }
   
   data->vectorName = AiNodeGetStr(node, SSTR::vector_aov_name);
   if (!data->vectorName.empty())
   {
      AiAOVRegister(data->vectorName, AI_TYPE_VECTOR, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasVector = true;
   }
   
   data->rgbName = AiNodeGetStr(node, SSTR::rgb_aov_name);
   if (!data->rgbName.empty())
   {
      AiAOVRegister(data->rgbName, AI_TYPE_RGB, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasRGB = true;
   }
   
   data->rgbaName = AiNodeGetStr(node, SSTR::rgba_aov_name);
   if (!data->rgbaName.empty())
   {
      AiAOVRegister(data->rgbaName, AI_TYPE_RGBA, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasRGBA = true;
   }
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
   
   if ((sg->Rt & AI_RAY_CAMERA))
   {
      if (data->hasBool && AiAOVEnabled(data->boolName, AI_TYPE_BOOLEAN))
      {
         AiAOVSetBool(sg, data->boolName, AiShaderEvalParamBool(p_bool_aov));
      }
      if (data->hasInt && AiAOVEnabled(data->intName, AI_TYPE_INT))
      {
         AiAOVSetInt(sg, data->intName, AiShaderEvalParamInt(p_int_aov));
      }
      if (data->hasFloat && AiAOVEnabled(data->floatName, AI_TYPE_FLOAT))
      {
         AiAOVSetFlt(sg, data->floatName, AiShaderEvalParamFlt(p_float_aov));
      }
      if (data->hasPoint2 && AiAOVEnabled(data->point2Name, AI_TYPE_POINT2))
      {
         AiAOVSetPnt2(sg, data->point2Name, AiShaderEvalParamPnt2(p_point2_aov));
      }
      if (data->hasPoint && AiAOVEnabled(data->pointName, AI_TYPE_POINT))
      {
         AiAOVSetPnt(sg, data->pointName, AiShaderEvalParamPnt(p_point_aov));
      }
      if (data->hasVector && AiAOVEnabled(data->vectorName, AI_TYPE_VECTOR))
      {
         AiAOVSetVec(sg, data->vectorName, AiShaderEvalParamVec(p_vector_aov));
      }
      if (data->hasRGB && AiAOVEnabled(data->rgbName, AI_TYPE_RGB))
      {
         AiAOVSetRGB(sg, data->rgbName, AiShaderEvalParamRGB(p_rgb_aov));
      }
      if (data->hasRGBA && AiAOVEnabled(data->rgbaName, AI_TYPE_RGBA))
      {
         AiAOVSetRGBA(sg, data->rgbaName, AiShaderEvalParamRGBA(p_rgba_aov));
      }
   }
   
   sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
}
