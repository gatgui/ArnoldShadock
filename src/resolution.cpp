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

AI_SHADER_NODE_EXPORT_METHODS(ResolutionMtd);

enum ResolutionParams
{
   p_target = 0,
   p_filename
};

enum TargetResolution
{
   TR_Render = 0,
   TR_Image
};

static const char* TargetResolutionNames[] = {"render", "image", NULL};

node_parameters
{
   AiParameterEnum(SSTR::target, TR_Render, TargetResolutionNames);
   AiParameterStr(SSTR::filename, "");
}

struct ResolutionData
{
   TargetResolution target;
   bool valid;
   float w;
   float h;
   bool evalFilename;
};

node_initialize
{
   AiNodeSetLocalData(node, new ResolutionData());
   AddMemUsage<ResolutionData>();
}

node_update
{
   ResolutionData *data = (ResolutionData*) AiNodeGetLocalData(node);
   
   AtNode *opts = AiUniverseGetOptions();
   
   data->target = (TargetResolution) AiNodeGetInt(node, SSTR::target);
   
   if (data->target == TR_Render)
   {
      data->w = float(AiNodeGetInt(opts, SSTR::xres));
      data->h = float(AiNodeGetInt(opts, SSTR::yres));
      data->valid = true;
   }
   else
   {
      data->evalFilename = AiNodeIsLinked(node, SSTR::filename);
      
      if (!data->evalFilename)
      {
         const char *filename = AiNodeGetStr(node, SSTR::filename);
         unsigned int w, h;
         
         data->valid = (filename && filename[0] != '\0' && AiTextureGetResolution(filename, &w, &h));
         
         if (data->valid)
         {
            data->w = float(w);
            data->h = float(h);
         }
      }
   }
}

node_finish
{
   ResolutionData *data = (ResolutionData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ResolutionData>();
}

shader_evaluate
{
   ResolutionData *data = (ResolutionData*) AiNodeGetLocalData(node);
   
   if (data->valid)
   {
      if (data->target == TR_Render)
      {
         sg->out.VEC.x = float(data->w);
         sg->out.VEC.y = float(data->h);
      }
      else
      {
         if (data->evalFilename)
         {
            unsigned int w, h;
            const char *filename = AiShaderEvalParamStr(p_filename);
            
            if (filename && filename[0] != '\0' && AiTextureGetResolution(filename, &w, &h))
            {
               sg->out.VEC.x = float(w);
               sg->out.VEC.y = float(h);
            }
            else
            {
               sg->out.VEC.x = 0.0f;
               sg->out.VEC.y = 0.0f;
            }
         }
         else
         {
            sg->out.VEC.x = data->w;
            sg->out.VEC.y = data->h;
         }
      }
      
      // Add aspect in Z channel
      sg->out.VEC.z = (sg->out.VEC.y >= AI_EPSILON ? (sg->out.VEC.x / sg->out.VEC.y) : 0.0f);
   }
   else
   {
      sg->out.VEC = AI_V3_ZERO;
   }
}
