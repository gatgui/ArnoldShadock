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

AI_SHADER_NODE_EXPORT_METHODS(ShadeMtd);

enum ShadeParams
{
   p_mode = 0,
   p_override_normal,
   p_normal
};

enum ShadeMode
{
   SM_DirectDiffuse = 0,
   SM_IndirectDiffuse
};

static const char* ShadeModeNames[] =
{
   "direct_diffuse",
   "indirect_diffuse",
   NULL
};

node_parameters
{
   AiParameterEnum(SSTR::mode, SM_DirectDiffuse, ShadeModeNames);
   AiParameterBool(SSTR::override_normal, false);
   AiParameterVec(SSTR::normal, 1.0f, 0.0f, 0.0f);
}

struct ShadeData
{
   int mode;
   bool overrideNormal;
};

node_initialize
{
   AiNodeSetLocalData(node, new ShadeData());
   AddMemUsage<ShadeData>();
}

node_update
{
   ShadeData *data = (ShadeData*) AiNodeGetLocalData(node);
   data->mode = AiNodeGetInt(node, SSTR::mode);
   data->overrideNormal = AiNodeGetBool(node, SSTR::override_normal);
}

node_finish
{
   ShadeData *data = (ShadeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<ShadeData>();
}

shader_evaluate
{
   ShadeData *data = (ShadeData*) AiNodeGetLocalData(node);
   
   AtVector N = (data->overrideNormal ? AiShaderEvalParamVec(p_normal) : sg->N);
   
   if (data->mode == SM_DirectDiffuse || data->mode == SM_IndirectDiffuse)
   {
      AtClosureList closures;
      AtBSDF *bsdf = AiOrenNayarBSDF(sg, AI_RGB_WHITE, N);
      AiBSDFSetDirectIndirect(bsdf, (data->mode == SM_DirectDiffuse ? 1.0 : 0.0),
                                    (data->mode == SM_DirectDiffuse ? 0.0 : 1.0));
      closures.add(bsdf);
      sg->out.CLOSURE() = closures;
   }
   else
   {
      sg->out.CLOSURE() = AiClosureEmission(sg, AI_RGB_BLACK);
   }
}
