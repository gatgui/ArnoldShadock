#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WriteMsgP2Mtd);

enum WriteMsgP2Params
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
   AiParameterPnt2(SSTR::msg_input, 0.0f, 0.0f);
   AiParameterEnum(SSTR::eval_order, EO_input_last, EvalOrderNames);
}

struct WriteMsgP2Data
{
   bool valid;
   AtString msgName;
   EvalOrder evalOrder;
};

node_initialize
{
   WriteMsgP2Data *data = new WriteMsgP2Data();
   AddMemUsage<WriteMsgP2Data>();
   
   data->valid = false;
   data->evalOrder = EO_input_last;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   WriteMsgP2Data *data = (WriteMsgP2Data*) AiNodeGetLocalData(node);
   
   data->evalOrder = (EvalOrder) AiNodeGetInt(node, SSTR::eval_order);
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
   data->valid = !data->msgName.empty();
}

node_finish
{
   WriteMsgP2Data *data = (WriteMsgP2Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<WriteMsgP2Data>();
}

shader_evaluate
{
   WriteMsgP2Data *data = (WriteMsgP2Data*) AiNodeGetLocalData(node);
   
   if (data->evalOrder == EO_input_first)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
   
   if (data->valid)
   {
      AiStateSetMsgPnt2(data->msgName, AiShaderEvalParamPnt2(p_msg_input));
   }
   
   if (data->evalOrder == EO_input_last)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}
