#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GetNodeMtd);

enum GetNodeParams
{
   p_target_node = 0,
   p_light_index
};

enum TargetNode
{
   TN_Shaded = 0,
   TN_Shader,
   TN_Proc,
   TN_Light,
   TN_Traced,
   TN_Probed
};

static const char* TargetNodeNames[] = {"shaded", "shader", "proc", "light", "traced", "probed", NULL};

node_parameters
{
   AiParameterEnum("target_node", TN_Shaded, TargetNodeNames);
   AiParameterInt("light_index", -1);
   
   AiMetaDataSetBool(mds, "target_node", "linkable", false);
}

node_initialize
{
}

node_update
{
   AiNodeSetLocalData(node, (void*) AiNodeGetInt(node, "target_node"));
}

node_finish
{
}

shader_evaluate
{
   TargetNode target_node = (TargetNode) size_t(AiNodeGetLocalData(node));
   
   switch (target_node)
   {
   case TN_Probed:
   case TN_Traced:
      {
         HitData *hd = 0;
         if (AiStateGetMsgPtr("agsb_trace_hit", (void**)&hd) && hd)
         {
            if (target_node == TN_Probed)
            {
               sg->out.PTR = (void*) (hd->isSG ? ((AtShaderGlobals*)hd->ptr)->Op : 0);
            }
            else
            {
               sg->out.PTR = (void*) (!hd->isSG ? ((AtScrSample*)hd->ptr)->obj : 0);
            }
         }
         else
         {
            sg->out.PTR = (void*) 0;
         }
      }
      break;
   case TN_Light:
      {
         int index = AiShaderEvalParamInt(p_light_index);
         if (index < 0)
         {
            sg->out.PTR = sg->Lp;
         }
         else if (index < sg->nlights)
         {
            sg->out.PTR = sg->lights[index];
         }
         else
         {
            sg->out.PTR = (void*) 0;
         }
      }
      break;
   case TN_Shader:
      sg->out.PTR = sg->shader;
      break;
   case TN_Proc:
      sg->out.PTR = sg->proc;
      break;
   case TN_Shaded:
   default:
      sg->out.PTR = sg->Op;
      break; 
   }
}
