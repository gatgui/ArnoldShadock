#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WriteMsgP3Mtd);

enum WriteMsgP3Params
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
   AiParameterPnt(SSTR::msg_input, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::eval_order, EO_input_last, EvalOrderNames);
}

struct NodeData
{
   bool valid;
   AtString msgName;
   EvalOrder evalOrder;
};

node_initialize
{
   NodeData *data = new NodeData();
   AddMemUsage<NodeData>();
   
   data->valid = false;
   data->evalOrder = EO_input_last;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->evalOrder = (EvalOrder) AiNodeGetInt(node, SSTR::eval_order);
   data->msgName = AiNodeGetStr(node, SSTR::msg_name);
   data->valid = !data->msgName.empty();
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
   
   if (data->evalOrder == EO_input_first)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
   
   if (data->valid)
   {
      AiStateSetMsgPnt(data->msgName, AiShaderEvalParamPnt(p_msg_input));
   }
   
   if (data->evalOrder == EO_input_last)
   {
      sg->out.RGBA = AiShaderEvalParamRGBA(p_input);
   }
}
