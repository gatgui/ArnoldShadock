#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WriteMsgC3Mtd);

enum WriteMsgC3Params
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
   AiParameterRGB(SSTR::msg_input, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::eval_order, EO_input_last, EvalOrderNames);
}

struct WriteMsgC3Data
{
   bool valid;
   AtString msgName;
   EvalOrder evalOrder;
};

node_initialize
{
   WriteMsgC3Data *data = new WriteMsgC3Data();
   AddMemUsage<WriteMsgC3Data>();
   
   data->valid = false;
   data->evalOrder = EO_input_last;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   WriteMsgC3Data *data = (WriteMsgC3Data*) AiNodeGetLocalData(node);
   
   data->evalOrder = (EvalOrder) AiNodeGetInt(node, SSTR::eval_order);
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
   data->valid = !data->msgName.empty();
}

node_finish
{
   WriteMsgC3Data *data = (WriteMsgC3Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<WriteMsgC3Data>();
}

shader_evaluate
{
   WriteMsgC3Data *data = (WriteMsgC3Data*) AiNodeGetLocalData(node);
   
   if (data->evalOrder == EO_input_first)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
   
   if (data->valid)
   {
      AiStateSetMsgRGB(data->msgName, AiShaderEvalParamRGB(p_msg_input));
   }
   
   if (data->evalOrder == EO_input_last)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}
