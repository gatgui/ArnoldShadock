#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WriteMsgsMtd);

enum WriteMsgsParams
{
   p_input = 0,
   p_bool_msg_name,
   p_bool_msg,
   p_int_msg_name,
   p_int_msg,
   p_float_msg_name,
   p_float_msg,
   p_point2_msg_name,
   p_point2_msg,
   p_point_msg_name,
   p_point_msg,
   p_vector_msg_name,
   p_vector_msg,
   p_rgb_msg_name,
   p_rgb_msg,
   p_rgba_msg_name,
   p_rgba_msg
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterStr("bool_msg_name", "");
   AiParameterBool("bool_msg", false);
   
   AiParameterStr("int_msg_name", "");
   AiParameterInt("int_msg", 0);
   
   AiParameterStr("float_msg_name", "");
   AiParameterFlt("float_msg", 0.0f);
   
   AiParameterStr("point2_msg_name", "");
   AiParameterPnt2("point2_msg", 0.0f, 0.0f);
   
   AiParameterStr("point_msg_name", "");
   AiParameterPnt("point_msg", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr("vector_msg_name", "");
   AiParameterVec("vector_msg", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr("rgb_msg_name", "");
   AiParameterRGB("rgb_msg", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr("rgba_msg_name", "");
   AiParameterRGBA("rgba_msg", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiMetaDataSetBool(mds, "bool_msg_name", "linkable", false);
   AiMetaDataSetBool(mds, "int_msg_name", "linkable", false);
   AiMetaDataSetBool(mds, "float_msg_name", "linkable", false);
   AiMetaDataSetBool(mds, "point2_msg_name", "linkable", false);
   AiMetaDataSetBool(mds, "point_msg_name", "linkable", false);
   AiMetaDataSetBool(mds, "vector_msg_name", "linkable", false);
   AiMetaDataSetBool(mds, "rgb_msg_name", "linkable", false);
   AiMetaDataSetBool(mds, "rgba_msg_name", "linkable", false);
   AiMetaDataSetBool(mds, "blend_opacity", "linkable", false);
}

struct WriteMsgsData
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
   WriteMsgsData *data = (WriteMsgsData*) AiMalloc(sizeof(WriteMsgsData));
   
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
   WriteMsgsData *data = (WriteMsgsData*) AiNodeGetLocalData(node);
   
   data->boolName = AiNodeGetStr(node, "bool_msg_name");
   data->hasBool = (strlen(data->boolName) > 0);
   
   data->intName = AiNodeGetStr(node, "int_msg_name");
   data->hasInt = (strlen(data->intName) > 0);
   
   data->floatName = AiNodeGetStr(node, "float_msg_name");
   data->hasFloat = (strlen(data->floatName) > 0);
   
   data->point2Name = AiNodeGetStr(node, "point2_msg_name");
   data->hasPoint2 = (strlen(data->point2Name) > 0);
   
   data->pointName = AiNodeGetStr(node, "point_msg_name");
   data->hasPoint = (strlen(data->pointName) > 0);
   
   data->vectorName = AiNodeGetStr(node, "vector_msg_name");
   data->hasVector = (strlen(data->vectorName) > 0);
   
   data->rgbName = AiNodeGetStr(node, "rgb_msg_name");
   data->hasRGB = (strlen(data->rgbName) > 0);
   
   data->rgbaName = AiNodeGetStr(node, "rgba_msg_name");
   data->hasRGBA = (strlen(data->rgbaName) > 0);
}

node_finish
{
   WriteMsgsData *data = (WriteMsgsData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   WriteMsgsData *data = (WriteMsgsData*) AiNodeGetLocalData(node);
   
   if (data->hasBool)
   {
      AiStateSetMsgBool(data->boolName, AiShaderEvalParamBool(p_bool_msg));
   }
   if (data->hasInt)
   {
      AiStateSetMsgInt(data->intName, AiShaderEvalParamInt(p_int_msg));
   }
   if (data->hasFloat)
   {
      AiStateSetMsgFlt(data->floatName, AiShaderEvalParamFlt(p_float_msg));
   }
   if (data->hasPoint2)
   {
      AiStateSetMsgPnt2(data->point2Name, AiShaderEvalParamPnt2(p_point2_msg));
   }
   if (data->hasPoint)
   {
      AiStateSetMsgPnt(data->pointName, AiShaderEvalParamPnt(p_point_msg));
   }
   if (data->hasVector)
   {
      AiStateSetMsgVec(data->vectorName, AiShaderEvalParamVec(p_vector_msg));
   }
   if (data->hasRGB)
   {
      AiStateSetMsgRGB(data->rgbName, AiShaderEvalParamRGB(p_rgb_msg));
   }
   if (data->hasRGBA)
   {
      AiStateSetMsgRGBA(data->rgbaName, AiShaderEvalParamRGBA(p_rgba_msg));
   }
   
   sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
}
