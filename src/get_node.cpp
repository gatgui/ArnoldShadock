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

AI_SHADER_NODE_EXPORT_METHODS(GetNodeMtd);

enum GetNodeParams
{
   p_target_node = 0,
};

enum TargetNode
{
   TN_Shaded = 0,
   TN_Shader,
   TN_Proc
   // TN_Traced,
   // TN_Probed
};

static const char* TargetNodeNames[] = {"shaded", "shader", "proc", /*"light", "traced", "probed",*/ NULL};

node_parameters
{
   AiParameterEnum(SSTR::target_node, TN_Shaded, TargetNodeNames);
}

struct GetNodeData
{
   int targetNode;
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
   // case TN_Probed:
   // case TN_Traced:
   //    {
   //       HitData *hd = 0;
   //       if (AiStateGetMsgPtr(SSTR::agsb_trace_hit, (void**)&hd) && hd)
   //       {
   //          if (data->targetNode == TN_Probed)
   //          {
   //             sg->out.PTR() = (void*) (hd->isSG ? ((AtShaderGlobals*)hd->ptr)->Op : 0);
   //          }
   //          else
   //          {
   //             sg->out.PTR() = (void*) (!hd->isSG ? ((AtScrSample*)hd->ptr)->obj : 0);
   //          }
   //       }
   //       else
   //       {
   //          sg->out.PTR() = (void*) 0;
   //       }
   //    }
   //    break;
   case TN_Shader:
      sg->out.PTR() = sg->shader;
      break;
   case TN_Proc:
      sg->out.PTR() = sg->proc;
      break;
   case TN_Shaded:
   default:
      sg->out.PTR() = sg->Op;
      break; 
   }
}
