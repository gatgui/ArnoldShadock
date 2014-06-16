#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(CameraMatrixMtd);

enum CameraMatrixParams
{
   p_camera = 0, // node? or by name
   p_camera_name,
   p_mode
};

// P' = P * world_to_camera * camera_to_sreen
// world_to_screen = world_to_camera * camera_to_sreen
enum MatrixMode
{
   MM_world_to_camera = 0,
   MM_camera_to_screen,
   MM_world_to_screen,
   MM_camera_to_world,
   MM_screen_to_camera,
   MM_screen_to_world
};

static const char* MatrixModeNames[] =
{
   "world_to_camera",
   "camera_to_screen",
   "world_to_screen",
   "camera_to_world",
   "screen_to_camera",
   "screen_to_world",
   NULL
};

struct NodeData
{
   AtNode *camera;
   MatrixMode mode;
};

node_parameters
{
   AiParameterNode("camera", NULL);
   AiParameterStr("camera_name", "");
   AiParameterEnum("mode", MM_world_to_camera, MatrixModeNames);
   
   AiMetaDataSetBool(mds, "mode", "linkable", false);
   AiMetaDataSetBool(mds, "camera_name", "linkable", false);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->mode = (MatrixMode) AiNodeGetInt(node, "mode");
   
   if (AiNodeIsLinked(node, "camera"))
   {
      data->camera = (AtNode*) AiNodeGetPtr(node, "camera");
   }
   else
   {
      const char *name = AiNodeGetStr(node, "camera_name");
      if (!name || strlen(name) == 0)
      {
         data->camera = AiUniverseGetCamera();
      }
      else
      {
         // search for named camera
         AtNode *n = AiNodeLookUpByName(name);
         if (n)
         {
            data->camera = n;
         }
         else
         {
            bool multi = false;
            data->camera = 0;
            
            AtNodeIterator *nit = AiUniverseGetNodeIterator(AI_NODE_CAMERA);
            while (!AiNodeIteratorFinished(nit))
            {
               n = AiNodeIteratorGetNext(nit);
               
               const char *curname = AiNodeGetName(n);
               if (!curname || strlen(curname) == 0)
               {
                  continue;
               }
               
               if (strstr(curname, name) != 0)
               {
                  if (data->camera != 0)
                  {
                     multi = true;
                     break;
                  }
                  else
                  {
                     data->camera = n;
                  }
               }
            }
            AiNodeIteratorDestroy(nit);
            
            if (!data->camera)
            {
               AiMsgWarning("[camera_matrix] No camera matching \"%s\" was found. Use render camera", name);
               data->camera = AiUniverseGetCamera();
            }
            else if (multi)
            {
               AiMsgWarning("[camera_matrix] Found several cameras matching \"%s\". Use first found: \"%s\"", name, AiNodeGetName(data->camera));
            }
         }
      }
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   sg->out.pMTX = (AtMatrix*) AiShaderGlobalsQuickAlloc(sg, sizeof(AtMatrix));
   
   switch (data->mode)
   {
   case MM_world_to_camera:
      AiWorldToCameraMatrix(data->camera, sg->time, *(sg->out.pMTX));
      break;
   case MM_camera_to_screen:
      {
         AtMatrix tmp0, tmp1, tmp2;
         AiWorldToCameraMatrix(data->camera, sg->time, tmp0);
         AiM4Invert(tmp0, tmp1);
         AiWorldToScreenMatrix(data->camera, sg->time, tmp2);
         AiM4Mult(*(sg->out.pMTX), tmp1, tmp2);
      }
      break;
   case MM_world_to_screen:
      AiWorldToScreenMatrix(data->camera, sg->time, *(sg->out.pMTX));
      break;
   case MM_camera_to_world:
      {
         AtMatrix tmp;
         AiWorldToCameraMatrix(data->camera, sg->time, tmp);
         AiM4Invert(tmp, *(sg->out.pMTX));
      }
      break;
   case MM_screen_to_camera:
      {
         AtMatrix tmp0, tmp1, tmp2;
         AiWorldToScreenMatrix(data->camera, sg->time, tmp0);
         AiM4Invert(tmp0, tmp1);
         AiWorldToCameraMatrix(data->camera, sg->time, tmp2);
         AiM4Mult(*(sg->out.pMTX), tmp1, tmp2);
      }
      break;
   case MM_screen_to_world:
      {
         AtMatrix tmp;
         AiWorldToScreenMatrix(data->camera, sg->time, tmp);
         AiM4Invert(tmp, *(sg->out.pMTX));
      }
      break;
   default:
      AiM4Identity(*(sg->out.pMTX));   
   }
   
}
