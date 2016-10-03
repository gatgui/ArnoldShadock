#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeP3Mtd);

enum SampleVolumeP3Params
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
   AiParameterPnt(SSTR::multiply, 1.0f, 1.0f, 1.0f);
   AiParameterPnt(SSTR::offset, 0.0f, 0.0f, 0.0f);
   AiParameterPnt(SSTR::_default, 0.0f, 0.0f, 0.0f);
   
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
   bool ignoreP;
   AtPoint P;
   Space space;
   bool isOffset;
   bool evalDefault;
   AtPoint _default;
   float bias;
   float gain;
   AtPoint multiply;
   AtPoint offset;
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
      data->_default = AiNodeGetPnt(node, SSTR::_default);
   }
   data->bias = AiNodeGetFlt(node, SSTR::bias);
   data->gain = AiNodeGetFlt(node, SSTR::gain);
   data->multiply = AiNodeGetPnt(node, SSTR::multiply);
   data->offset = AiNodeGetPnt(node, SSTR::offset);
   data->space = (Space) AiNodeGetInt(node, SSTR::P_space);
   data->isOffset = AiNodeGetBool(node, SSTR::P_is_offset);
   data->evalP = AiNodeIsLinked(node, SSTR::P);
   if (!data->evalP)
   {
      data->P = AiNodeGetPnt(node, SSTR::P);
      data->ignoreP = (data->isOffset && AiV3IsZero(data->P));
   }
   else
   {
      data->ignoreP = false;
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
   
   AtPoint oldP = sg->P;
   AtPoint oldPo = sg->Po;
   
   if (!data->ignoreP)
   {
      AtPoint P;
      bool updateP = true;
      
      if (data->evalP)
      {
         P = AiShaderEvalParamPnt(p_P);
         updateP = (!data->isOffset || !AiV3IsZero(P));
      }
      else
      {
         P = data->P;
      }

      if (updateP)
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
   }

   AtPoint out = AI_P3_ZERO;

   if (!AiVolumeSamplePnt(data->field, data->interpolation, &out))
   {
      sg->out.PNT = (data->evalDefault ? AiShaderEvalParamPnt(p_default) : data->_default);
   }
   else
   {
      out.x = GAIN(BIAS(out.x, data->bias), data->gain);
      out.y = GAIN(BIAS(out.y, data->bias), data->gain);
      out.z = GAIN(BIAS(out.z, data->bias), data->gain);
      sg->out.PNT = data->offset + data->multiply * out;
   }

   sg->P = oldP;
   sg->Po = oldPo;
}
