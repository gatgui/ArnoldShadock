#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WriteAOVIMtd);

enum WriteAOVIParams
{
   p_input = 0,
   p_aov_name,
   p_aov_input,
   p_blend_opacity,
   p_eval_order
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterStr(SSTR::aov_name, "");
   AiParameterInt(SSTR::aov_input, 0);
   AiParameterBool(SSTR::blend_opacity, true);
   AiParameterEnum(SSTR::eval_order, EO_input_first, EvalOrderNames);
}

struct WriteAOVIData
{
   AtString aovName;
   bool valid;
   EvalOrder evalOrder;
};

node_initialize
{
   WriteAOVIData *data = new WriteAOVIData();
   AddMemUsage<WriteAOVIData>();
   
   data->valid = false;
   data->evalOrder = EO_input_first;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   WriteAOVIData *data = (WriteAOVIData*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
   data->evalOrder = (EvalOrder) AiNodeGetInt(node, SSTR::eval_order);
   
   if (!data->aovName.empty())
   {
      bool blend = AiNodeGetBool(node, SSTR::blend_opacity);
      data->valid = AiAOVRegister(data->aovName, AI_TYPE_INT, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
   }
   else
   {
      data->valid = false;
   }
}

node_finish
{
   WriteAOVIData *data = (WriteAOVIData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<WriteAOVIData>();
}

shader_evaluate
{
   WriteAOVIData *data = (WriteAOVIData*) AiNodeGetLocalData(node);
   
   if (data->evalOrder == EO_input_first)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
   
   if ((sg->Rt & AI_RAY_CAMERA))
   {
      if (data->valid && AiAOVEnabled(data->aovName, AI_TYPE_INT))
      {
         AiAOVSetInt(sg, data->aovName, AiShaderEvalParamInt(p_aov_input));
      }
   }
   
   if (data->evalOrder == EO_input_last)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}
