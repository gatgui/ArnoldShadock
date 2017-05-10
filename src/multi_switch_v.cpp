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

AI_SHADER_NODE_EXPORT_METHODS(MultiSwitchVMtd);

enum MultiSwitchVParams
{
   p_which = 0,
   p_input0,
   p_input1,
   p_input2,
   p_input3,
   p_input4,
   p_input5,
   p_input6,
   p_input7,
   p_input8,
   p_input9,
   p_input10,
   p_input11,
   p_input12,
   p_input13,
   p_input14,
   p_input15,
};

node_parameters
{
   AiParameterInt("which", 0);
   AiParameterVec("input0", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input3", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input4", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input5", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input6", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input7", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input8", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input9", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input10", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input11", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input12", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input13", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input14", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input15", 0.0f, 0.0f, 0.0f);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   int which = AiShaderEvalParamInt(p_which);
   sg->out.VEC = AiShaderEvalParamVec(p_input0 + (which < 0 ? 0 : (which > 15 ? 15 : which)));
}
