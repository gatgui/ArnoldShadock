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

AI_SHADER_NODE_EXPORT_METHODS(MultiSwitchClMtd);

enum MultiSwitchClParams
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
   AiParameterClosure("input0");
   AiParameterClosure("input1");
   AiParameterClosure("input2");
   AiParameterClosure("input3");
   AiParameterClosure("input4");
   AiParameterClosure("input5");
   AiParameterClosure("input6");
   AiParameterClosure("input7");
   AiParameterClosure("input8");
   AiParameterClosure("input9");
   AiParameterClosure("input10");
   AiParameterClosure("input11");
   AiParameterClosure("input12");
   AiParameterClosure("input13");
   AiParameterClosure("input14");
   AiParameterClosure("input15");
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
   sg->out.CLOSURE() = AiShaderEvalParamClosure(p_input0 + (which < 0 ? 0 : (which > 15 ? 15 : which)));
}
