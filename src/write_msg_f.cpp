#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WriteMsgFMtd);

enum WriteMsgFParams
{
   p_input = 0,
   p_msg_name,
   p_msg_input,
   p_eval_order
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterStr(SSTR::msg_name, "");
   AiParameterFlt(SSTR::msg_input, 0.0f);
   AiParameterEnum(SSTR::eval_order, EO_input_last, EvalOrderNames);
}

struct WriteMsgFData
{
   bool valid;
   AtString msgName;
   EvalOrder evalOrder;
};

node_initialize
{
   WriteMsgFData *data = new WriteMsgFData();
   AddMemUsage<WriteMsgFData>();
   
   data->valid = false;
   data->evalOrder = EO_input_last;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   WriteMsgFData *data = (WriteMsgFData*) AiNodeGetLocalData(node);
   
   data->evalOrder = (EvalOrder) AiNodeGetInt(node, SSTR::eval_order);
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
   data->valid = !data->msgName.empty();
}

node_finish
{
   WriteMsgFData *data = (WriteMsgFData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<WriteMsgFData>();
}

shader_evaluate
{
   WriteMsgFData *data = (WriteMsgFData*) AiNodeGetLocalData(node);
   
   if (data->evalOrder == EO_input_first)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
   
   if (data->valid)
   {
      AiStateSetMsgFlt(data->msgName, AiShaderEvalParamFlt(p_msg_input));
   }
   
   if (data->evalOrder == EO_input_last)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}
