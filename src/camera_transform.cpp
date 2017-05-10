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
#include <vector>
#include <string>

AI_SHADER_NODE_EXPORT_METHODS(CameraTransformMtd);

enum CameraTransformParams
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

node_parameters
{
   AiParameterNode(SSTR::camera, NULL);
   AiParameterStr(SSTR::camera_name, "");
   AiParameterEnum(SSTR::mode, MM_world_to_camera, MatrixModeNames);
}

struct CameraTransformData
{
   AtNode *camera;
   MatrixMode mode;
};

node_initialize
{
   AiNodeSetLocalData(node, new CameraTransformData());
   AddMemUsage<CameraTransformData>();
}

node_update
{
   CameraTransformData *data = (CameraTransformData*) AiNodeGetLocalData(node);
   
   data->mode = (MatrixMode) AiNodeGetInt(node, SSTR::mode);
   
   if (AiNodeIsLinked(node, SSTR::camera))
   {
      data->camera = (AtNode*) AiNodeGetPtr(node, SSTR::camera);
   }
   else
   {
      const char *name = AiNodeGetStr(node, SSTR::camera_name);
      
      if (!name || strlen(name) == 0)
      {
         data->camera = AiUniverseGetCamera();
      }
      else
      {
         std::vector<std::string> names;
         std::string pattern = name;
         
         size_t p0 = pattern.find_first_not_of(" \t");
         size_t p1 = pattern.find_first_of(" \t", p0);
         
         while (p1 != std::string::npos)
         {
            names.push_back(pattern.substr(p0, p1 - p0));
            
            p0 = pattern.find_first_not_of(" \t", p1);
            p1 = pattern.find_first_of(" \t", p0);
         }
         
         names.push_back(pattern.substr(p0));
         
         size_t nnames = names.size();
         
         bool multi = false;
         data->camera = 0;
         
         // search for named camera
         for (size_t i=0; i<nnames; ++i)
         {
            name = names[i].c_str();
            
            AtNode *n = AiNodeLookUpByName(name);
            
            if (n)
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
            else
            {
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
               
               if (multi)
               {
                  break;
               }
            }
         }
         
         if (!data->camera)
         {
            AiMsgWarning("[camera_transform] No camera matching \"%s\" was found. Use render camera", name);
            data->camera = AiUniverseGetCamera();
         }
         else if (multi)
         {
            AiMsgWarning("[camera_transform] Found several cameras matching \"%s\". Use first found: \"%s\"", name, AiNodeGetName(data->camera));
         }
      }
   }
}

node_finish
{
   CameraTransformData *data = (CameraTransformData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<CameraTransformData>();
}

shader_evaluate
{
   CameraTransformData *data = (CameraTransformData*) AiNodeGetLocalData(node);
   
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
