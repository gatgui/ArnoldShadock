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

AI_SHADER_NODE_EXPORT_METHODS(WriteAOVP2Mtd);

enum WriteAOVP2Params
{
   p_input = 0,
   p_aov_name,
   p_aov_input,
   p_blend_opacity,
   p_eval_order
};

node_parameters
{
   AiParameterRGBA("input", 0.0f, 0.0f, 0.0f, 1.0f);
   
   AiParameterStr(SSTR::aov_name, "");
   AiParameterVec2(SSTR::aov_input, 0.0f, 0.0f);
   AiParameterBool(SSTR::blend_opacity, true);
   AiParameterEnum(SSTR::eval_order, EO_input_first, EvalOrderNames);
}

struct WriteAOVP2Data
{
   AtString aovName;
   bool valid;
   EvalOrder evalOrder;
};

node_initialize
{
   WriteAOVP2Data *data = new WriteAOVP2Data();
   AddMemUsage<WriteAOVP2Data>();
   
   data->valid = false;
   data->evalOrder = EO_input_first;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   WriteAOVP2Data *data = (WriteAOVP2Data*) AiNodeGetLocalData(node);
   
   data->aovName = AiNodeGetStr(node, SSTR::aov_name);
   data->evalOrder = (EvalOrder) AiNodeGetInt(node, SSTR::eval_order);
   
   if (!data->aovName.empty())
   {
      bool blend = AiNodeGetBool(node, SSTR::blend_opacity);
      data->valid = AiAOVRegister(data->aovName, AI_TYPE_VECTOR2, (blend ? AI_AOV_BLEND_OPACITY : AI_AOV_BLEND_NONE));
   }
   else
   {
      data->valid = false;
   }
}

node_finish
{
   WriteAOVP2Data *data = (WriteAOVP2Data*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<WriteAOVP2Data>();
}

shader_evaluate
{
   WriteAOVP2Data *data = (WriteAOVP2Data*) AiNodeGetLocalData(node);
   
   if (data->evalOrder == EO_input_first)
   {
      sg->out.RGBA() = AiShaderEvalParamRGBA(p_input);
   }
   
   if ((sg->Rt & AI_RAY_CAMERA))
   {
      if (data->valid && AiAOVEnabled(data->aovName, AI_TYPE_VECTOR2))
      {
         AiAOVSetVec2(sg, data->aovName, AiShaderEvalParamVec2(p_aov_input));
      }
   }
   
   if (data->evalOrder == EO_input_last)
   {
      sg->out.RGBA() = AiShaderEvalParamRGBA(p_input);
   }
}
