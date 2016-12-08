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
   AiParameterEnum(SSTR::target_node, TN_Shaded, TargetNodeNames);
   AiParameterInt(SSTR::light_index, -1);
}

struct GetNodeData
{
   int targetNode;
   int lightIndex;
};

node_initialize
{
   AiNodeSetLocalData(node, new GetNodeData());
   AddMemUsage<GetNodeData>();
}

node_update
{
   GetNodeData *data = (GetNodeData*) AiNodeGetLocalData(node);
   
   data->targetNode = AiNodeGetInt(node, SSTR::target_node);
   data->lightIndex = AiNodeGetInt(node, SSTR::light_index);
}

node_finish
{
   GetNodeData *data = (GetNodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<GetNodeData>();
}

shader_evaluate
{
   GetNodeData *data = (GetNodeData*) AiNodeGetLocalData(node);
   
   switch (data->targetNode)
   {
   case TN_Probed:
   case TN_Traced:
      {
         HitData *hd = 0;
         if (AiStateGetMsgPtr(SSTR::agsb_trace_hit, (void**)&hd) && hd)
         {
            if (data->targetNode == TN_Probed)
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
         if (data->lightIndex < 0)
         {
            sg->out.PTR = sg->Lp;
         }
         else if (data->lightIndex < sg->nlights)
         {
            sg->out.PTR = sg->lights[data->lightIndex];
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
