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
   
   AiParameterStr(SSTR::bool_msg_name, "");
   AiParameterBool("bool_msg", false);
   
   AiParameterStr(SSTR::int_msg_name, "");
   AiParameterInt("int_msg", 0);
   
   AiParameterStr(SSTR::float_msg_name, "");
   AiParameterFlt("float_msg", 0.0f);
   
   AiParameterStr(SSTR::point2_msg_name, "");
   AiParameterPnt2("point2_msg", 0.0f, 0.0f);
   
   AiParameterStr(SSTR::point_msg_name, "");
   AiParameterPnt("point_msg", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::vector_msg_name, "");
   AiParameterVec("vector_msg", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::rgb_msg_name, "");
   AiParameterRGB("rgb_msg", 0.0f, 0.0f, 0.0f);
   
   AiParameterStr(SSTR::rgba_msg_name, "");
   AiParameterRGBA("rgba_msg", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiMetaDataSetBool(mds, SSTR::bool_msg_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::int_msg_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::float_msg_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::point2_msg_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::point_msg_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::vector_msg_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::rgb_msg_name, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::rgba_msg_name, SSTR::linkable, false);
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
   
   data->boolName = AiNodeGetStr(node, SSTR::bool_msg_name);
   data->hasBool = !data->boolName.empty();
   
   data->intName = AiNodeGetStr(node, SSTR::int_msg_name);
   data->hasInt = !data->intName.empty();
   
   data->floatName = AiNodeGetStr(node, SSTR::float_msg_name);
   data->hasFloat = !data->floatName.empty();
   
   data->point2Name = AiNodeGetStr(node, SSTR::point2_msg_name);
   data->hasPoint2 = !data->point2Name.empty();
   
   data->pointName = AiNodeGetStr(node, SSTR::point_msg_name);
   data->hasPoint = !data->pointName.empty();
   
   data->vectorName = AiNodeGetStr(node, SSTR::vector_msg_name);
   data->hasVector = !data->vectorName.empty();
   
   data->rgbName = AiNodeGetStr(node, SSTR::rgb_msg_name);
   data->hasRGB = !data->rgbName.empty();
   
   data->rgbaName = AiNodeGetStr(node, SSTR::rgba_msg_name);
   data->hasRGBA = !data->rgbaName.empty();
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
