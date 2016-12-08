#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(NodeAttrBMtd);

enum NodeAttrBParams
{
   p_target = 0,
   p_light_index,
   p_target_node,
   p_attribute,
   p_user_defined,
   p_default
};

node_parameters
{
   AiParameterEnum(SSTR::target, NAT_surface, NodeAttrTargetNames);
   AiParameterInt(SSTR::light_index, -1);
   AiParameterNode(SSTR::target_node, NULL);
   AiParameterStr(SSTR::attribute, "");
   AiParameterBool(SSTR::user_defined, true);
   AiParameterBool(SSTR::_default,  false);
}

struct NodeAttrBData
{
   NodeAttrTarget target;
   int light_index;
   AtString attribute;
   bool user_defined;
   bool linked;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeAttrBData());
   AddMemUsage<NodeAttrBData>();
}

node_update
{
   NodeAttrBData *data = (NodeAttrBData*) AiNodeGetLocalData(node);
   data->target = (NodeAttrTarget) AiNodeGetInt(node, SSTR::target);
   data->light_index = AiNodeGetInt(node, SSTR::light_index);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->user_defined = AiNodeGetBool(node, SSTR::user_defined);
   data->linked = AiNodeIsLinked(node, SSTR::target_node);
}

node_finish
{
   NodeAttrBData *data = (NodeAttrBData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeAttrBData>();
}

shader_evaluate
{
   NodeAttrBData *data = (NodeAttrBData*) AiNodeGetLocalData(node);
   
   AtNode *src = NULL;
   
   switch (data->target)
   {
   case NAT_surface:
      src = sg->Op;
      break;
   case NAT_shader:
      src = sg->shader;
      break;
   case NAT_light:
      if (data->light_index < 0)
      {
         src = sg->Lp;
      }
      else if (data->light_index < sg->nlights)
      {
         src = sg->lights[data->light_index];
      }
      break;
   case NAT_procedural:
      src = sg->proc;
      break;
   case NAT_custom:
      if (data->linked)
      {
         src = (AtNode*) AiShaderEvalParamPtr(p_target_node);
      }
   default:
      break;
   }
   
   if (!src)
   {
      sg->out.BOOL = AiShaderEvalParamBool(p_default);
   }
   else
   {
      int type = AI_TYPE_UNDEFINED;
      
      if (data->user_defined)
      {
         const AtUserParamEntry *pe = AiNodeLookUpUserParameter(src, data->attribute);
         
         if (pe && AiUserParamGetCategory(pe) == AI_USERDEF_CONSTANT)
         {
            type = AiUserParamGetType(pe);
         }
      }
      else
      {
         const AtParamEntry *pe = AiNodeEntryLookUpParameter(AiNodeGetNodeEntry(src), data->attribute);
         
         if (pe)
         {
            type = AiParamGetType(pe);
         }
      }
      
      if (type == AI_TYPE_BOOLEAN)
      {
         sg->out.BOOL = AiNodeGetBool(src, data->attribute);
      }
      else
      {
         sg->out.BOOL = AiShaderEvalParamBool(p_default);
      }
   }
}
