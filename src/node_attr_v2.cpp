/*
MIT License
Copyright (c) 2014~ Gaetan Guidet
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(NodeAttrV2Mtd);

enum NodeAttrV2Params
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
   AiParameterVec2(SSTR::_default, 0.0f, 0.0f);
}

struct NodeAttrV2Data
{
   NodeAttrTarget target;
   int light_index;
   AtString attribute;
   bool user_defined;
   bool linked;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeAttrV2Data());
   AddMemUsage<NodeAttrV2Data>();
}

node_update
{
   NodeAttrV2Data *data = (NodeAttrV2Data*) AiNodeGetLocalData(node);
   data->target = (NodeAttrTarget) AiNodeGetInt(node, SSTR::target);
   data->light_index = AiNodeGetInt(node, SSTR::light_index);
   data->attribute = AiNodeGetStr(node, SSTR::attribute);
   data->user_defined = AiNodeGetBool(node, SSTR::user_defined);
   data->linked = AiNodeIsLinked(node, SSTR::target_node);
}

node_finish
{
   NodeAttrV2Data *data = (NodeAttrV2Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeAttrV2Data>();
}

shader_evaluate
{
   NodeAttrV2Data *data = (NodeAttrV2Data*) AiNodeGetLocalData(node);
   
   AtNode *src = NULL;
   AtLightSample ls;
   
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
         AiLightsGetSample(sg, ls);
         src = (AtNode *)ls.Lp;
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
      sg->out.VEC2() = AiShaderEvalParamVec2(p_default);
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
      
      if (type == AI_TYPE_VECTOR2)
      {
         sg->out.VEC2() = AiNodeGetVec2(src, data->attribute);
      }
      else
      {
         sg->out.VEC2() = AiShaderEvalParamVec2(p_default);
      }
   }
}
