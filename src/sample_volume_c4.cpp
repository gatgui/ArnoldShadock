#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeC4Mtd);

enum SampleVolumeC4Params
{
   p_field = 0,
   p_interpolation,
   p_P,
   p_P_space,
   p_P_is_offset,
   p_bias,
   p_gain,
   p_multiply,
   p_offset,
   p_default
};

enum Space
{
   S_world = 0,
   S_object
};

static const char* SpaceNames[] =
{
   "world",
   "object",
   NULL
};

node_parameters
{
   AiParameterStr(SSTR::field, "");
   AiParameterEnum(SSTR::interpolation, VI_linear, VolumeInterpNames);
   AiParameterPnt(SSTR::P, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::P_space, S_world, SpaceNames);
   AiParameterBool(SSTR::P_is_offset, true);
   AiParameterFlt(SSTR::bias, 0.5f);
   AiParameterFlt(SSTR::gain, 0.5f);
   AiParameterRGBA(SSTR::multiply, 1.0f, 1.0f, 1.0f, 1.0f);
   AiParameterRGBA(SSTR::offset, 0.0f, 0.0f, 0.0f, 0.0f);
   AiParameterRGBA(SSTR::_default, 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiMetaDataSetBool(mds, SSTR::field, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::interpolation, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::bias, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::gain, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::multiply, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::offset, SSTR::linkable, false);
}

struct NodeData
{
   AtString field;
   int interpolation;
   bool evalP;
   AtPoint P;
   Space space;
   bool isOffset;
   bool evalDefault;
   AtRGBA _default;
   float bias;
   float gain;
   AtRGBA multiply;
   AtRGBA offset;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->field = AiNodeGetStr(node, SSTR::field);
   data->interpolation = AiNodeGetInt(node, SSTR::interpolation);
   data->evalDefault = AiNodeIsLinked(node, SSTR::_default);
   if (!data->evalDefault)
   {
      data->_default = AiNodeGetRGBA(node, SSTR::_default);
   }
   data->bias = AiNodeGetFlt(node, SSTR::bias);
   data->gain = AiNodeGetFlt(node, SSTR::gain);
   data->multiply = AiNodeGetRGBA(node, SSTR::multiply);
   data->offset = AiNodeGetRGBA(node, SSTR::offset);
   data->evalP = AiNodeIsLinked(node, SSTR::P);
   if (!data->evalP)
   {
      data->P = AiNodeGetPnt(node, SSTR::P);
   }
   data->space = (Space) AiNodeGetInt(node, SSTR::P_space);
   data->isOffset = AiNodeGetBool(node, SSTR::P_is_offset);
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
   
   AtPoint oldP = sg->P;
   AtPoint oldPo = sg->Po;
   
   AtPoint P = (data->evalP ? AiShaderEvalParamPnt(p_P) : data->P);

   if (!data->isOffset || !AiV3IsZero(P))
   {
      switch (data->space)
      {
      case S_object:
         sg->Po = (data->isOffset ? (sg->Po + P) : P);
         AiM4PointByMatrixMult(&(sg->P), sg->M, &(sg->Po));
         break;
      case S_world:
      default:
         sg->P = (data->isOffset ? (sg->P + P) : P);
         AiM4PointByMatrixMult(&(sg->Po), sg->Minv, &(sg->P));
      }
   }

   AtRGBA out = AI_RGBA_BLACK;

   if (!AiVolumeSampleRGBA(data->field, data->interpolation, &out))
   {
      sg->out.RGBA = (data->evalDefault ? AiShaderEvalParamRGBA(p_default) : data->_default);
   }
   else
   {
      out.r = GAIN(BIAS(out.r, data->bias), data->gain);
      out.g = GAIN(BIAS(out.g, data->bias), data->gain);
      out.b = GAIN(BIAS(out.b, data->bias), data->gain);
      out.a = GAIN(BIAS(out.a, data->bias), data->gain);
      sg->out.RGBA = data->offset + data->multiply * out;
   }

   sg->P = oldP;
   sg->Po = oldPo;
}
