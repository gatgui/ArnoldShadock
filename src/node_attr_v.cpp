#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(NodeAttrVMtd);

enum NodeAttrVParams
{
   p_target_node = 0,
   p_attribute,
   p_user_defined,
   p_default
};

node_parameters
{
   AiParameterNode(SSTR::target_node, NULL);
   AiParameterStr(SSTR::attribute, "");
   AiParameterBool(SSTR::user_defined, true);
   AiParameterVec(SSTR::_default, 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, SSTR::attribute, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::user_defined, SSTR::linkable, false);
}

struct NodeData
{
   AtString attribute;
   bool user_defined;
   bool linked;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->user_defined = AiNodeGetBool(node, SSTR::user_defined);
   data->linked = AiNodeIsLinked(node, SSTR::target_node);
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
   
   sg->out.VEC = AI_V3_ZERO;
   
   if (!data->linked)
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default);
   }
   else
   {
      AtNode *src = (AtNode*) AiShaderEvalParamPtr(p_target_node);
      
      if (!src)
      {
         sg->out.VEC = AiShaderEvalParamVec(p_default);
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
         
         switch (type)
         {
         case AI_TYPE_POINT:
            // AiNodeGetLink doesn't have a AtString version yet
            //AtNode *psrc = AiNodeGetLink(src, data->attribute);
            //if (psrc) { AiShaderEvaluate(psrc, sg) } else
            sg->out.VEC = AiNodeGetPnt(src, data->attribute);
            break;
         case AI_TYPE_VECTOR:
            // AiNodeGetLink doesn't have a AtString version yet
            //AtNode *psrc = AiNodeGetLink(src, data->attribute);
            //if (psrc) { AiShaderEvaluate(psrc, sg) } else
            sg->out.VEC = AiNodeGetVec(src, data->attribute);
            break;
         default:
            sg->out.VEC = AiShaderEvalParamVec(p_default);
         }
      }
   }
}
