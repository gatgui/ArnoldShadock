#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleVolumeC4Mtd);

enum SampleVolumeC4Params
{
   p_field = 0,
   p_interpolation,
   p_P,
   p_P_space,
   p_P_is_offset,
   p_pre_multiply,
   p_pre_add,
   p_blend_mode,
   p_blend,
   p_blend_min,
   p_blend_max,
   p_bias,
   p_gain,
   p_post_multiply,
   p_post_add,
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
   AiParameterRGBA(SSTR::pre_multiply, 1.0f, 1.0f, 1.0f, 1.0f);
   AiParameterRGBA(SSTR::pre_add, 0.0f, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::blend_mode, BM_none, BlendModeNames);
   AiParameterFlt(SSTR::blend, 1.0f);
   AiParameterRGBA(SSTR::blend_min, 0.0f, 0.0f, 0.0f, 1.0f);
   AiParameterRGBA(SSTR::blend_max, 1.0f, 1.0f, 1.0f, 1.0f);
   AiParameterFlt(SSTR::bias, 0.5f);
   AiParameterFlt(SSTR::gain, 0.5f);
   AiParameterRGBA(SSTR::post_multiply, 1.0f, 1.0f, 1.0f, 1.0f);
   AiParameterRGBA(SSTR::post_add, 0.0f, 0.0f, 0.0f, 0.0f);
   AiParameterRGBA(SSTR::_default, 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiMetaDataSetBool(mds, SSTR::field, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::interpolation, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::pre_multiply, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::pre_add, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::blend_mode, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::bias, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::gain, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::post_multiply, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::post_add, SSTR::linkable, false);
   
   AiMetaDataSetFlt(mds, SSTR::blend, "min", 0.0f);
   AiMetaDataSetFlt(mds, SSTR::blend, "max", 1.0f);
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
   AtRGBA _default;
   float bias;
   float gain;
   AtRGBA preMult;
   AtRGBA preAdd;
   BlendMode blendMode;
   AtRGBA postMult;
   AtRGBA postAdd;
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
   data->preMult = AiNodeGetRGBA(node, SSTR::pre_multiply);
   data->preAdd = AiNodeGetRGBA(node, SSTR::pre_add);
   data->blendMode = (BlendMode) AiNodeGetInt(node, SSTR::blend_mode);
   data->bias = AiNodeGetFlt(node, SSTR::bias);
   data->gain = AiNodeGetFlt(node, SSTR::gain);
   data->postMult = AiNodeGetRGBA(node, SSTR::post_multiply);
   data->postAdd = AiNodeGetRGBA(node, SSTR::post_add);
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

   AtRGBA out = AI_RGBA_BLACK;

   if (!AiVolumeSampleRGBA(data->field, data->interpolation, &out))
   {
      sg->out.RGBA = (data->evalDefault ? AiShaderEvalParamRGBA(p_default) : data->_default);
   }
   else
   {
      // Pre mult/add
      out = data->preAdd + data->preMult * out;
      
      // Blend
      switch (data->blendMode)
      {
      case BM_min_field:
         {
            float blend = AiShaderEvalParamFlt(p_blend);
            if (blend < 1.0f)
            {
               AtRGBA bmin = AiShaderEvalParamRGBA(p_blend_min);
               out = bmin + blend * (out - bmin);
            }
         }
         break;
      case BM_field_max:
         {
            float blend = AiShaderEvalParamFlt(p_blend);
            if (blend > 0.0f)
            {
               AtRGBA bmax = AiShaderEvalParamRGBA(p_blend_max);
               out = out + blend * (bmax - out);
            }
         }
         break;
      case BM_field_as_blender:
         {
            AtRGBA bmin = AiShaderEvalParamRGBA(p_blend_min);
            AtRGBA bmax = AiShaderEvalParamRGBA(p_blend_max);
            out.r = bmin.r + out.r * (bmax.r - bmin.r);
            out.g = bmin.g + out.g * (bmax.g - bmin.g);
            out.b = bmin.b + out.b * (bmax.b - bmin.b);
            out.a = bmin.a + out.a * (bmax.a - bmin.a);
         }
      default:
         break;
      }
      
      // Gain/Bias
      out.r = GAIN(BIAS(out.r, data->bias), data->gain);
      out.g = GAIN(BIAS(out.g, data->bias), data->gain);
      out.b = GAIN(BIAS(out.b, data->bias), data->gain);
      out.a = GAIN(BIAS(out.a, data->bias), data->gain);
      
      // Post mult/add
      sg->out.RGBA = data->postAdd + data->postMult * out;
   }

   sg->P = oldP;
   sg->Po = oldPo;
}
