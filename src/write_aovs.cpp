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
   
   AiParameterStr("bool_aov_name", "");
   AiParameterBool("bool_aov", false);
   
   AiParameterStr("int_aov_name", "");
   AiParameterInt("int_aov", 0);
   
   AiParameterStr("float_aov_name", "");
   AiParameterFlt("float_aov", 0.0f);
   
   AiParameterStr("point2_aov_name", "");
   AiParameterPnt2("point2_aov", 0.0f, 0.0f);
   
   AiParameterStr("point_aov_name", "");
   AiParameterPnt("point_aov", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr("vector_aov_name", "");
   AiParameterVec("vector_aov", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr("rgb_aov_name", "");
   AiParameterRGB("rgb_aov", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr("rgba_aov_name", "");
   AiParameterRGBA("rgba_aov", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterBool("blend_opacity", true);
   
   AiMetaDataSetBool(mds, "bool_aov_name", "linkable", false);
   AiMetaDataSetBool(mds, "int_aov_name", "linkable", false);
   AiMetaDataSetBool(mds, "float_aov_name", "linkable", false);
   AiMetaDataSetBool(mds, "point2_aov_name", "linkable", false);
   AiMetaDataSetBool(mds, "point_aov_name", "linkable", false);
   AiMetaDataSetBool(mds, "vector_aov_name", "linkable", false);
   AiMetaDataSetBool(mds, "rgb_aov_name", "linkable", false);
   AiMetaDataSetBool(mds, "rgba_aov_name", "linkable", false);
   AiMetaDataSetBool(mds, "blend_opacity", "linkable", false);
}

struct WriteAOVsData
{
   bool hasBool;
   bool hasInt;
   bool hasFloat;
   bool hasPoint2;
   bool hasPoint;
   bool hasVector;
   bool hasRGB;
   bool hasRGBA;
   const char *boolName;
   const char *intName;
   const char *floatName;
   const char *point2Name;
   const char *pointName;
   const char *vectorName;
   const char *rgbName;
   const char *rgbaName;
};

node_initialize
{
   WriteAOVsData *data = (WriteAOVsData*) AiMalloc(sizeof(WriteAOVsData));
   
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
   WriteAOVsData *data = (WriteAOVsData*) AiNodeGetLocalData(node);
   
   bool blend = AiNodeGetBool(node, "blend_opacity");
   
   data->boolName = AiNodeGetStr(node, "bool_aov_name");
   if (strlen(data->boolName) > 0)
   {
      AiAOVRegister(data->boolName, AI_TYPE_BOOLEAN, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasBool = true;
   }
   
   data->intName = AiNodeGetStr(node, "int_aov_name");
   if (strlen(data->intName) > 0)
   {
      AiAOVRegister(data->intName, AI_TYPE_INT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasInt = true;
   }
   
   data->floatName = AiNodeGetStr(node, "float_aov_name");
   if (strlen(data->floatName) > 0)
   {
      AiAOVRegister(data->floatName, AI_TYPE_FLOAT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasFloat = true;
   }
   
   data->point2Name = AiNodeGetStr(node, "point2_aov_name");
   if (strlen(data->point2Name) > 0)
   {
      AiAOVRegister(data->point2Name, AI_TYPE_POINT2, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasPoint2 = true;
   }
   
   data->pointName = AiNodeGetStr(node, "point_aov_name");
   if (strlen(data->pointName) > 0)
   {
      AiAOVRegister(data->pointName, AI_TYPE_POINT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasPoint = true;
   }
   
   data->vectorName = AiNodeGetStr(node, "vector_aov_name");
   if (strlen(data->vectorName) > 0)
   {
      AiAOVRegister(data->vectorName, AI_TYPE_VECTOR, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasVector = true;
   }
   
   data->rgbName = AiNodeGetStr(node, "rgb_aov_name");
   if (strlen(data->rgbName) > 0)
   {
      AiAOVRegister(data->rgbName, AI_TYPE_RGB, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasRGB = true;
   }
   
   data->rgbaName = AiNodeGetStr(node, "rgba_aov_name");
   if (strlen(data->rgbaName) > 0)
   {
      AiAOVRegister(data->rgbaName, AI_TYPE_RGBA, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
      data->hasRGBA = true;
   }
}

node_finish
{
   WriteAOVsData *data = (WriteAOVsData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   WriteAOVsData *data = (WriteAOVsData*) AiNodeGetLocalData(node);
   
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
